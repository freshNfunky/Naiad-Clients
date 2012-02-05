// -----------------------------------------------------------------------------
//
// main.cc
//
// The Naiad Command-Line Solver Client
//
// Copyright (c) 2009-2011 Exotic Matter AB.  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// * Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// * Neither the name of Exotic Matter AB nor its contributors may be used to
// endorse or promote products derived from this software without specific
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// -----------------------------------------------------------------------------

// Naiad Interface headers
#include <Ni.h>
#include <NiNg.h>

// Naiad Graph API (for body restart)
#include <NgBodyOp.h>

// Naiad Base API (for EMP reading etc)
#include <NbEmpSequenceReader.h>
#include <NbEmpReader.h>
#include <NbParticleShape.h>
#include <NbFilename.h>

// c++
#include <iomanip>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>

#ifdef WINDOWS
#include <Windows.h>
#endif

// -----------------------------------------------------------------------------

class StatFileCallback : public NtGraphCallback
{
public:
    virtual bool
    beginStep(const NtTimeBundle& tb)
    { return true; }

    virtual bool
    beginFrame(const NtTimeBundle& tb)
    {
        EM_TIMER_SAMPLE(frameStart);
#ifdef EM_TIMER
        _frameStart=frameStart;
#endif

        NI_INFO("----------------------------");
        NI_INFO("Solving frame " << tb.frame);
        NI_INFO("----------------------------");

        NtString logFile(NiQueryLogFile());
#ifdef WINDOWS
        unsigned int last_slash(logFile.str().find_last_of('\\'));
#else
        unsigned int last_slash(logFile.str().find_last_of('/'));
#endif
        std::string statPath, statFileName;
        if(last_slash==std::string::npos) {
            statPath=".";            
            statFileName=logFile;
        } else {
            statPath=logFile.str().substr(0,last_slash);            
            statFileName=logFile.str().substr(last_slash+1);            
        }
        std::stringstream ss;
#ifdef WINDOWS
        ss << statPath << "\\stat";
        const int result = CreateDirectory(ss.str().c_str(), NULL);
#else
        ss << statPath << "/stat";
        ::mkdir(ss.str().c_str(),S_IFDIR|S_IRWXU|S_IRWXO|S_IRWXG);
#endif
        unsigned int last_dot(statFileName.find_last_of('.'));
        std::string statPrefix;
        if(last_dot==std::string::npos)
            statPrefix=statFileName;
        else
            statPrefix=statFileName.substr(0,last_dot);    
        
#ifdef WINDOWS
        ss << "\\" << statPrefix << "." << std::setw(4) << std::setfill('0') 
           << tb.frame << ".stat";
#else
        ss << "/" << statPrefix << "." << std::setw(4) << std::setfill('0') 
           << tb.frame << ".stat";
#endif
        
        em::open_stat(ss.str());
        std::string sfile(ss.str());
        NI_INFO("Statfile: " << sfile);        
        EM_STAT("Frame " << tb.frame);
        return true;
    }

    virtual bool
    beginTimestep(const NtTimeBundle& tb)
    {
        NI_INFO(">>>>> Timestep " << tb.timestep << 
               " | current time: " << tb.time << " sec "
               " | dt " << tb.dt << " | <<<<<");

        EM_STAT(std::endl);
        EM_STAT("----------------------------------");
        EM_STAT("Timestep " << tb.timestep);
        EM_STAT("----------------------------------");
        return true;
    }

    virtual bool
    endTimestep(const NtTimeBundle& tb)
    { return true; }

    virtual bool
    endFrame(const NtTimeBundle& tb)
    {        
        EM_STAT(std::endl);
        em::close_stat();
        EM_TIMER_SAMPLE(frameEnd);
        EM_TIMER_CALC(total,_frameStart,frameEnd);
        EM_TIMER_LOG("Frame solved in " << total << " sec");
        return true;
    }

    virtual bool
    endStep(const NtTimeBundle& tb)
    { return true; }

    virtual bool
    reset()
    { return true; }

    virtual void
    beginOp(const Nb::TimeBundle& tb, const Nb::String& opInstance) {}

    virtual void
    endOp(const Nb::TimeBundle& tb, const Nb::String& opInstance) 
    { std::cerr << "Naiad: Stepped Op " << opInstance << std::endl; }

private:
    double _frameStart, _frameEnd;
};

// -----------------------------------------------------------------------------

inline void
print_usage()
{    
    std::cerr 
        << "Usage: naiad file1.ni file2.ni ... [options]"
        << std::endl
        << std::endl
        << "The time-step graphs encoded in file1.ni, file2.ni ... sequence "
        << "are run in order listed (e.g., file1.ni before file2.ni)"
        << std::endl
        << std::endl
        << "Options may be any of the following:" 
        << std::endl
        << "--frames first last          * solve only the frames 'first'..'last'" 
        << std::endl
        << "--verbose                    * verbose (diagnostic) output in log" 
        << std::endl
        << "--threads n                  * force thread count to 'n'"
        << std::endl
        << "--[global var] n             * force 'global variable' to value 'n'"
        << std::endl
        << "--activate[op]               * force activate 'op'"
        << std::endl
        << "--restart n emp-seq          * restart bodies at frame 'n', using "
        << " body state stored in the EMP sequence 'emp-seq'."
        << std::endl
        << "--help                       * print this information" 
        << std::endl
        << std::endl;
}

// -----------------------------------------------------------------------------

void
restartBodies(const NtString empSequenceName, 
              const NtInt    firstFrame, 
              const NtInt    empPadding)
{
    NtStringList opInstances=NiQueryOpNames(NI_INSTANCE,NI_BODY_OP);
    
    // construct an EmpReader corresponding to the actual restart frame
    Nb::String restartEmpName = 
        Nb::sequenceToFilename(NiEvalParam1s("Global.Project Path"),
                               empSequenceName, 
                               firstFrame, 
                               -1, 
                               empPadding);
    Nb::EmpReader empReader(restartEmpName,"*");
    const double  empTime = empReader.time()-1e-12;

    // now make a sequence reader, and step back from that frame - that may
    // be a time-step or a full frame, but whatever it is, that's where we
    // get our initial state from...
    Nb::EmpSequenceReader empSequence;    
    empSequence.setProjectPath(NiEvalParam1s("Global.Project Path"));
    empSequence.setSequenceName(empSequenceName);
    empSequence.setPadding(empPadding);
    empSequence.stepBack(empTime);
    
    // restart all bodies found in EMP...
    for(int b=0; b<empSequence.bodyCount(); ++b) {
        Nb::Body* body = empSequence.cloneBody(b);
        if(body->restartOpName()!="") {
            try {
                Ng::BodyOp* bodyOp = NiMutableBodyOp(body->restartOpName());
                bodyOp->param1e("Construction")->setExpr("Raw Disk Cache");
                bodyOp->param1s("EMP Cache")->setExpr(
                    empSequence.prevEmpName()
                    );
                bodyOp->param1s("Body Name")->setExpr(body->name());
                NB_INFO("Restarting body '" << body->name() << 
                        "' using Origin Op: " << bodyOp->name());
            }
            catch(std::exception& e) {
                NB_WARNING("Unable to restart body '" << body->name() <<
                           "':" << e.what());
            }
        }
        delete body;
    }
}

// -----------------------------------------------------------------------------

int
main(int argc, char* argv[])
{
    // begin by displaying our proud banner!
    std::cout << std::endl
              << "Naiad Command-Line Client" << std::endl
              << "Copyright (c) 2009-2011 Exotic Matter AB." << std::endl
              << std::endl;

    NtString     empSequenceName;
    NtInt        empPadding(4);
    NtStringList niFiles;

    // parse command-line arguments    
    if(argc<2) {
        NiSetVerboseLevel(NI_SILENT);            
        std::cerr << NiQueryVersion();
        print_usage();
        NiEnd();
        return 0;
    }

    NtString              first_frame, last_frame, thread_count;
    NtString              restartFrame, restartSeq;
    std::vector<NtString> global_override, global_expr, active_ops;

    try {
        for(int i(1); i<argc; i++) {
            const NtString x(argv[i]);
            if(x[0]=='-' && x[1]=='-') {                
                if(x=="--help") {
                    print_usage();
                    return 0;
                } else if(x=="--verbose") {
                    NiSetVerboseLevel(NI_VERBOSE);
                } else if(x=="--frames") {
                    first_frame=argv[i+1];
                    last_frame=argv[i+2];
                    i += 2;
                } else if(x=="--restart") {
                    restartFrame=argv[i+1];
                    restartSeq=argv[i+2];
                    i += 2;
                } else if(x=="--threads") {               
                    thread_count=argv[i+1];
                    i++;
                } else if(x=="--emp-padding") {               
                    empPadding=std::atoi(argv[i+1]);
                    i++;
                } else if(x.str().find("--[")!=std::string::npos) {
                    NtString xx(x);
                    while(xx.str().find(']')==std::string::npos && i<argc) {
                        xx += " ";
                        xx += argv[++i];
                    }
                    if(i==argc)
                        NI_THROW("Global override: no end brace detected");
                    global_override.push_back(xx.str().substr(3,xx.size()-4));
                    global_expr.push_back(argv[++i]);
                    NI_INFO("** Global Override [" 
                           << global_override.back() << ": " 
                           << global_expr.back() << "]");
                } else if(x.str().find("--activate[")!=std::string::npos) {
                    NtString xx(x);
                    while(xx.str().find(']')==std::string::npos && i<argc) {
                        xx += " ";
                        xx += argv[++i];
                    }
                    if(i==argc)
                        NI_THROW("BodyOp activation: no end brace detected");
                    active_ops.push_back(xx.str().substr(11,xx.size()-12));
                } else
                    NI_THROW("Illegal command-line option: " << x);
            } else if(x.size()>3         && 
                      x[x.size()-3]=='.' && 
                      x[x.size()-2]=='n' &&
                      x[x.size()-1]=='i') {                
                niFiles.push_back(x);
            }
        }

        if(niFiles.empty()) {
            std::cerr << "No NI file specified. Stop teasing Naiad..."
                      << std::endl;
            return 0;
        }

        for(int nif=0; nif<niFiles.size(); nif++) {

            if(NiBegin()==NI_FALSE)
                continue;

            NiParseFileWithLog(niFiles[nif]);        

            if(!thread_count.str().empty())
                NiSetParam("Global.Thread Count",thread_count);

            for(unsigned int i(0); i<global_override.size(); ++i)
                NiSetParam("Global."+global_override[i],global_expr[i]);
            
            for(unsigned int i(0); i<active_ops.size(); ++i)
                NiSetOpState(active_ops[i],NI_ACTIVE);
        
            StatFileCallback* statFileCallback(new StatFileCallback());
            NiRegisterGraphCallback(statFileCallback);

            NI_INFO("Project: " << NiEvalParam1s("Global.Project Path"));
            NI_INFO("Simulation started on " << em::calendar_time());

            if(!first_frame.str().empty())
                NiSetParam("Global.First Frame",first_frame);

            if(!last_frame.str().empty())
                NiSetParam("Global.Last Frame",last_frame);

            int startFrame;
            if(restartSeq!="") {
                startFrame = atoi(restartFrame.c_str());
                restartBodies(restartSeq,startFrame,empPadding);
            } else {
                startFrame = NiEvalParam1i("Global.First Frame");
            }

            // step the graph!
            NiReset(startFrame,NI_TRUE);
            NiStepTo(NiEvalParam1i("Global.Last Frame"));

            NI_INFO("Simulation ended on " << em::calendar_time());

            NiEnd();
            delete statFileCallback;
        }
    }
    catch(std::bad_alloc) {
        std::cerr 
            << "Naiad: You made her run out of memory. She is not impressed."
            << std::endl;
        return 1;
    }
    catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    catch(...) {
        std::cerr 
            << "Naiad: Unhandled internal error. Naiad is not impressed."
            << std::endl;
        return 1;
    }

    return 0;
}

// -----------------------------------------------------------------------------
