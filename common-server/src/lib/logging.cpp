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


#include "p4pserver/logging.h"

#include <log4cpp/PatternLayout.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/SyslogAppender.hh>
#include <iostream>

void init_logger(unsigned int level, const std::string& ident)
{
	switch (level)
	{
	case 0: log4cpp::Category::setRootPriority(log4cpp::Priority::CRIT);  break;
	case 1: log4cpp::Category::setRootPriority(log4cpp::Priority::ERROR); break;
	case 2: log4cpp::Category::setRootPriority(log4cpp::Priority::WARN);  break;
	case 3: log4cpp::Category::setRootPriority(log4cpp::Priority::INFO);  break;
	default:log4cpp::Category::setRootPriority(log4cpp::Priority::DEBUG); break;
	}

	if (!ident.empty())
	{
		log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
		layout->setConversionPattern("%c: %m%n");

		log4cpp::Appender* appender = new log4cpp::SyslogAppender("SYSLOG", ident);
		appender->setLayout(layout);
		log4cpp::Category::getRoot().removeAllAppenders();
		log4cpp::Category::getRoot().setAppender(appender);
	}
}

void logging_init() __attribute__((constructor));
void logging_init()
{
	log4cpp::Appender* appender = new log4cpp::OstreamAppender("STDERR", &std::cerr);
	appender->setLayout(new log4cpp::BasicLayout());
	log4cpp::Category::getRoot().removeAllAppenders();
	log4cpp::Category::getRoot().setAppender(appender);
}

void logging_deinit() __attribute__((destructor));
void logging_deinit()
{
	/* log4cpp::Category::shutdown(); */
}
