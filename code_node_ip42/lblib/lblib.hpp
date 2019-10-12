

#ifndef __LBLIB_HPP_INCLUDED__
#define __LBLIB_HPP_INCLUDED__

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <sys/time.h>
#include <string.h>
#define NP_FILE_LENGTH 10



class LoadBalancingTask {
    private: 
        //taskNumber between 0 and 6
        int taskNumber; 
        //1 if executed internally, 0 if executed externally
        int internal;
        //Linux System PID returned by the function callTask()
        int PID; 
        //time when process was started, aquired with gettimeofday() - function
        timeval timeStamp;
    
    public:
        
        //setter functions
        //dont use these functions, always use constructor
        //still in public for debugging - probably should be moved to private
        //set taskNumber to a number from 0 to 6 - use with care
        void setTaskNumber(int task) {
            if(task < 0 || task > 6) {
                std::cout << "[LoadBalancingTask Class]: no task recoginsed with this number \n" << std::endl;
                return;
            }
            taskNumber = task;
        }
        
        //set the internal variable to either 1 or 0 - use with care
        void setInternal(int place) {
            if(place == 1 || place == 0) {
                internal = place;
            }
            else {
                std::cout << "[LoadBalancingTask Class]: internal must be boolean \n" << std::endl;
                return;
            }
        }
        
        //set the PID manually, use with care
        void setPID(int linuxPID) {
            //PID of -1 is a task, that is executed exteranly
            if (linuxPID < 2 && linuxPID != -1) {
                std::cout << "[LoadBalancingTask Class]: no a valid PID \n" << std::endl;
                return;
            }
            PID = linuxPID;
        }
        
        //set time stamp, use with care - this could reset the time of creation if called not correctly
        void setTimeStamp(void) {
            gettimeofday(&timeStamp, NULL);
        }
        
        //getter functions
        //return task number, also check for correct creation
        int getTaskNumber(void) {
            if(taskNumber == -1) {
                std::cout << "[LoadBalancingTask Class]: taskNumber not created correctly \n" << std::endl;
            }
            return taskNumber;
        }
        
        //return the internal variable, also check for correct creation
        int getInternal(void) {
            if(internal == -1) {
                std::cout << "[LoadBalancingTask Class]: internal not created correctly \n" << std::endl;
            }
            return internal;
        }
        
        //return teh stored pid, also check for correct values
        int getPID(void) {
            if(PID == -10) {
                std::cout << "[LoadBalancingTask Class]: PID not created correctly \n" << std::endl;
            }
            return PID;
        }
        
        //return the time of creation (in seconds)
        double getTimeStamp(void) {
             return ((double) (timeStamp.tv_usec) / 1000000 + (double) (timeStamp.tv_sec));
        }
        
        //return the age of the object upon this exact moment
        double lifeTime(void) {
            timeval timeStamp2;
            gettimeofday(&timeStamp2, NULL);
            return (((double) (timeStamp2.tv_usec) / 1000000 + (double) (timeStamp2.tv_sec)) - getTimeStamp());
        }
        
        //constructor
        //default constructor, don't use this
        LoadBalancingTask() {
            internal = -1;
            PID = -10;
            taskNumber = -1;
        }
        
        //use this constructor instead
        LoadBalancingTask(int task, int place, int linuxPID) {
            setTaskNumber(task);
            setInternal(place);
            setPID(linuxPID);
            setTimeStamp();
        }
        
        //destructor, return the livetime of this object in seconds
        ~LoadBalancingTask() {
            
        }
};


float readNP(int taskNumber);
float readLoad(int minPriority);
float readLoadIM(int minPriority);
bool amIInternally (void);




#endif