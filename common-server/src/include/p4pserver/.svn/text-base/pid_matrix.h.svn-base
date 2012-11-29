/*
 * Copyright (c) 2008,2009, Yale Laboratory of Networked Systems
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *     * Neither the name of Yale University nor the names of its contributors may
 *       be used to endorse or promote products derived from this software without
 *       specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef PID_MATRIX_H
#define PID_MATRIX_H

#include <math.h>
#include <iterator>
#include <boost/foreach.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/shared_ptr.hpp>
#include <p4p/pid.h>
#include <p4pserver/local_obj.h>
#include <p4pserver/dist_obj.h>
#include <p4pserver/compiler.h>

class p4p_common_server_EXPORT VCost
{
public:
	VCost() : value_(NAN) {}
	VCost(double value) : value_(value) {}
	operator double() const { return value_; }
private:
#ifdef P4P_CLUSTER
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & value_;
	}
#endif

	double value_;
};

template <class T>
class PIDMatrixBase;

class p4p_common_server_EXPORT IndexedPID : public p4p::PID
{
public:
	unsigned int get_index() const { return index_; }
private:
#ifdef P4P_CLUSTER
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<p4p::PID>(*this);
		ar & index_;
	}
#endif

	template <class T>
	friend class PIDMatrixBase;

	IndexedPID() : index_(UINT_MAX) {}
	IndexedPID(const p4p::PID&  l, unsigned int i) : p4p::PID(l), index_(i) {}
	unsigned int index_;
};

typedef boost::multi_index::multi_index_container<
		IndexedPID,
		boost::multi_index::indexed_by<
			boost::multi_index::random_access<>,
			boost::multi_index::ordered_unique< boost::multi_index::identity<p4p::PID> >
		>
	> PIDMatrixPIDs;
typedef PIDMatrixPIDs::nth_index<0>::type PIDMatrixPIDsByIdx;
typedef PIDMatrixPIDs::nth_index<1>::type PIDMatrixPIDsByLoc;


template <class MatrixType>
class p4p_common_server_ex_EXPORT PIDMatrixBase : public DistributedObject
{
public:
	PIDMatrixBase()
	{
		matrix_ = new MatrixType();
		pids_ = new PIDMatrixPIDs();
		pids_by_idx_ = &pids_->get<0>();
		pids_by_pid_ = &pids_->get<1>();

		after_construct();
	}

	virtual ~PIDMatrixBase()
	{
		before_destruct();

		delete pids_;
		delete matrix_;
	}

	bool add_pid(const p4p::PID&  pid, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());

		/*
		 * Check if the pid already exists
		 */
		if (pids_by_pid_->find(pid) != pids_by_pid_->end())
			return false;

		/*
		 * Collect the existing pids and add in the new pid
		 */
		p4p::PIDSet pid_set;
		std::copy(pids_->begin(), pids_->end(), std::inserter(pid_set, pid_set.end()));
		pid_set.insert(pid);

		/*
		 * Set the new pids
		 */
		set_pids(pid_set, lock);
		return true;
	}

	void add_pids(const p4p::PIDSet& pids, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());

		/*
		 * Find out new pids
		 */
		p4p::PIDSet new_pids;
		std::set_difference(pids.begin(), pids.end(),
				    pids_->begin(), pids_->end(),
				    std::inserter(new_pids, new_pids.end()));
		if (new_pids.empty()) return;

		/*
		* Collect existing pids and add in new ones
		*/
		p4p::PIDSet pid_set;
		std::set_union(pids_->begin(), pids_->end(),
			       new_pids.begin(), new_pids.end(),
			       std::inserter(pid_set, pid_set.end()));

		/*
		 * Set the new pids
		 */
		set_pids(pid_set, lock);
	}

	void add_pids(const p4p::PIDVector& pids, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());

		p4p::PIDSet pid_set;
		std::copy(pids.begin(), pids.end(), std::inserter(pid_set, pid_set.end()));
		add_pids(pid_set, lock);
	}

	bool remove_pid(const p4p::PID&  pid, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());

		if (pids_by_pid_->find(pid) == pids_by_pid_->end())
			return false;

		p4p::PIDSet pid_set;
		std::copy(pids_->begin(), pids_->end(), std::inserter(pid_set, pid_set.end()));
		pid_set.erase(pid);

		set_pids(pid_set, lock);
		return true;
	}

	void set_pids(const p4p::PIDVector& locs, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());

		p4p::PIDSet pid_set;
		std::copy(locs.begin(), locs.end(), std::inserter(pid_set, pid_set.end()));
		set_pids(pid_set, lock);
	}

	void set_pids(const p4p::PIDSet& locs, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());

		/*
		 * Create a new matrix for the data
		 */
		MatrixType* new_matrix = new MatrixType();
		new_matrix->resize(locs.size(), locs.size(), false);

		/*
		 * Create the new set of pids/indices
		 */
		PIDMatrixPIDs* new_pids = new PIDMatrixPIDs();
		PIDMatrixPIDsByIdx* new_pids_by_idx = &new_pids->get<0>();
		PIDMatrixPIDsByLoc* new_pids_by_loc = &new_pids->get<1>();

		new_pids_by_idx->reserve(locs.size());
		BOOST_FOREACH(const p4p::PID&  pid, locs)
		{
			new_pids_by_idx->push_back(IndexedPID(pid, new_pids_by_idx->size()));
		}

		/*
		 * Copy data to the new matrix
		 */
		unsigned int n = matrix_->size1();
		for (unsigned int i = 0; i < n; ++i)
		{
			for (unsigned int j = 0; j < n; ++j)
			{
				/* If there was no entry in the existing matrix, we don't
				   need to worry about copying. */
				const VCost& p = (*matrix_)(i, j);
				if (std::isnan((double)p))
					continue;

				/* Find the pids in the new matrix */
				PIDMatrixPIDsByLoc::const_iterator new_i = new_pids_by_loc->find(pids_by_idx_->at(i));
				if (new_i == new_pids_by_loc->end())
					continue;
				PIDMatrixPIDsByLoc::const_iterator new_j = new_pids_by_loc->find(pids_by_idx_->at(j));
				if (new_j == new_pids_by_loc->end())
					continue;

				(*new_matrix)(new_i->get_index(), new_j->get_index()) = p;
			}
		}

		/*
		 * We have the write lock, so its okay to leave the
		 * data members temporarily as we're replacing them.
		 */
		delete matrix_;
		matrix_ = new_matrix;
		delete pids_;
		pids_ = new_pids;
		pids_by_idx_ = new_pids_by_idx;
		pids_by_pid_ = new_pids_by_loc;

		changed(lock);
	}

	double get(const IndexedPID&  src, const IndexedPID&  dst, const ReadableLock& lock, const VCost& dflt = VCost()) const
	{
		lock.check_read(get_local_mutex());
		const VCost& result = (*matrix_)(src.get_index(), dst.get_index());
		return std::isnan((double)result) ? dflt : result;
	}

	double get_by_pid(const p4p::PID&  src, const p4p::PID&  dst, const ReadableLock& lock, const VCost& dflt = VCost()) const
	{
		lock.check_read(get_local_mutex());
		PIDMatrixPIDsByLoc::const_iterator src_i = pids_by_pid_->find(src);
		if (src_i == pids_by_pid_->end())
			return dflt;
		PIDMatrixPIDsByLoc::const_iterator dst_i = pids_by_pid_->find(dst);
		if (dst_i == pids_by_pid_->end())
			return dflt;
		return get(*src_i, *dst_i, lock, dflt);
	}

	void set(const IndexedPID&  src, const IndexedPID&  dst, double value, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());
		(*matrix_)(src.get_index(), dst.get_index()) = value;
		changed(lock);
	}

	bool set_by_pid(const p4p::PID&  src, const p4p::PID&  dst, double value, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());
		PIDMatrixPIDsByLoc::const_iterator src_i = pids_by_pid_->find(src);
		if (src_i == pids_by_pid_->end())
			return false;
		PIDMatrixPIDsByLoc::const_iterator dst_i = pids_by_pid_->find(dst);
		if (dst_i == pids_by_pid_->end())
			return false;
		set(*src_i, *dst_i, value, lock);
		return true;
	}

	const PIDMatrixPIDsByIdx& get_pids_vector(const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return *pids_by_idx_;
	}

	const PIDMatrixPIDsByLoc& get_pids_set(const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return *pids_by_pid_;
	}
	unsigned int get_num_rows(const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return matrix_->size1();
	}

	unsigned int get_num_cols(const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return matrix_->size2();
	}

#ifdef P4P_CLUSTER
	virtual void do_load(InputArchive& stream, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());
		MatrixType& ref_matrix = *matrix_;
		PIDMatrixPIDs& ref_pids = *pids_;
		stream >> ref_matrix;
		stream >> ref_pids;
		changed(lock);
	}

	virtual void do_save(OutputArchive& stream, const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		const MatrixType& ref_matrix = *matrix_;
		const PIDMatrixPIDs& ref_pids = *pids_;
		stream << ref_matrix;
		stream << ref_pids;
	}
#endif

protected:
	virtual LocalObjectPtr create_empty_instance() const { return boost::shared_ptr< PIDMatrixBase<MatrixType> >(new PIDMatrixBase<MatrixType>()); }

	virtual DistributedObjectPtr do_copy_properties(const ReadableLock& lock)
	{
		lock.check_read(get_local_mutex());

		PIDMatrixBase<MatrixType>* new_obj = new PIDMatrixBase<MatrixType>();
		*new_obj->pids_ = *pids_;
		*new_obj->matrix_ = *matrix_;
		return boost::shared_ptr< PIDMatrixBase<MatrixType> >(new_obj);
	}

private:
	PIDMatrixBase(const PIDMatrixBase<MatrixType>&) { throw std::runtime_error("Not Implemented"); }
	PIDMatrixBase<MatrixType>& operator=(const PIDMatrixBase<MatrixType>&) { throw std::runtime_error("Not Implemented"); }

	PIDMatrixPIDs* pids_;
	PIDMatrixPIDsByIdx* pids_by_idx_;
	PIDMatrixPIDsByLoc* pids_by_pid_;

	MatrixType* matrix_;
};

typedef PIDMatrixBase< boost::numeric::ublas::mapped_matrix<VCost> > SparsePIDMatrix;
typedef boost::shared_ptr<SparsePIDMatrix> SparsePIDMatrixPtr;
typedef boost::shared_ptr<const SparsePIDMatrix> SparsePIDMatrixConstPtr;

typedef PIDMatrixBase< boost::numeric::ublas::matrix<VCost> > PIDMatrix;
typedef boost::shared_ptr<PIDMatrix> PIDMatrixPtr;
typedef boost::shared_ptr<const PIDMatrix> PIDMatrixConstPtr;

#endif
