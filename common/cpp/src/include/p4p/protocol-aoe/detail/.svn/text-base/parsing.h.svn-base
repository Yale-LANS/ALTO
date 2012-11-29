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


#ifndef P4P_AOEProtocol_PARSING_H
#define P4P_AOEProtocol_PARSING_H

#include <p4p/protocol/detail/parsing.h>
#include <p4p/protocol-aoe/weights.h>
#include <p4p/protocol-aoe/swarm_state.h>
#include <p4p/protocol-aoe/pid_class_pcts.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace protocol {
namespace aoe {
namespace detail {

class p4p_common_cpp_EXPORT RequestStateWriter : public p4p::protocol::detail::RequestWriter
{
public:
	RequestStateWriter(const SwarmState& state)
		: state_(state),
		  cur_(state_.get_pids().begin()),
		  last_(state_.get_pids().end())
	{}

	virtual bool produce() throw (P4PProtocolError)
	{
		for ( ; cur_ != last_; ++cur_)
		{
			const PID& p = *cur_;
			if (!write(p)) return false;
			if (!write('\t')) return false;
			if (!write(state_.get_ul_cap(p))) return false;
			if (!write('\t')) return false;
			if (!write(state_.get_dl_cap(p))) return false;
			if (!write('\t')) return false;
			if (!write(state_.get_seeds(p))) return false;
			if (!write('\t')) return false;
			if (!write(state_.get_leechers(p))) return false;
			if (!write('\t')) return false;
			if (!write(state_.get_ul_rate(p))) return false;
			if (!write('\t')) return false;
			if (!write(state_.get_dl_rate(p))) return false;
			if (!write('\t')) return false;
			if (!write(state_.get_active_peers(p))) return false;
			if (!write("\r\n")) return false;
			write_checkpoint();
		}
			return true;
	}
private:
	SwarmState state_;
	SwarmState::PIDList::const_iterator cur_, last_;
};

class p4p_common_cpp_EXPORT ResponseWeightsReader : public p4p::protocol::detail::ResponseReader
{
public:
	ResponseWeightsReader(WeightsMap& result, PIDClassPercentages& result_pcts)
		: result_(result),
		  result_pcts_(result_pcts)
	{}
	virtual size_t consume(bool finished) throw (P4PProtocolError)
	{
		std::string::size_type pos = 0;
		while (true)
		{
			std::string::size_type cur_pos = pos;

			std::string token;

			/* Read source */
			PID src;
			if (!read_token(cur_pos, finished, src, false))
				break;

			/* Read intrapid percentage */
			double intrapid_pct;
			if (!(read_token(cur_pos, finished, token, true) && token == "p" && read_token(cur_pos, finished, intrapid_pct, true)))
				break;

			/* Read intradomain percentage */
			double intradomain_pct;
			if (!(read_token(cur_pos, finished, token, true) && token == "i" && read_token(cur_pos, finished, intradomain_pct, true)))
				break;

			/* Read interdomain percentage */
			double interdomain_pct;
			if (!(read_token(cur_pos, finished, token, true) && token == "e" && read_token(cur_pos, finished, interdomain_pct, true)))
				break;

			result_pcts_.add(src, intrapid_pct, intradomain_pct, interdomain_pct);

			/* Read count of destinations */
			unsigned int dst_count;
			if (!read_token(cur_pos, finished, dst_count, true))
				break;

			/* Read destinations and weights */
			bool failed = false;
			for (unsigned int i = 0; i < dst_count; ++i)
			{
				/* Read destination */
				PID dst;
				if (!read_token(cur_pos, finished, dst, true))
				{
					failed = true;
					break;
				}

				/* Read weight */
				double weight;
				if (!read_token(cur_pos, finished, weight, true))
				{
					failed = true;
					break;
				}

				result_.set(src, dst, weight);
			}

			if (failed)
				break;

			pos = cur_pos;
		}

		return pos;
	}
private:
	WeightsMap& result_;
	PIDClassPercentages& result_pcts_;
};

};
};
};
};

#endif

