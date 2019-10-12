

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
        //linux priority niceness: -20 = highest priority, 19 = lowest priority
        int linuxNicenessValue;
        //linux priority class: 
        //class 0 => 19 - 15
        //class 1 => 14 - 10
        //class 2 => 9 - 5
        //class 3 => 4 - 0
        //class 4 => -1 - -5
        //class 5 => -6 - -10
        //class 6 => -11 - -15
        //class 7 => -16 - -20 //class not available for load balancing tasks
        int linuxPriotiryClass;
        //node priority element of this task
        int nodePriority;
    
    private:
        
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
                std::cout << "[LoadBalancingTask Class]: not a valid PID \n" << std::endl;
                return;
            }
            PID = linuxPID;
        }
        
        //set time stamp, use with care - this could reset the time of creation if called not correctly
        void setTimeStamp(void) {
            gettimeofday(&timeStamp, NULL);
        }
        
        //set the linuxPriorityClass to a specific value
        void setlinuxPriorityClass(int priorityClass) {
            if(priorityClass < 0 || priorityClass > 6) {
                std::cout << "[LoadBalancingTask Class]: not a valid priorityClass, range from 0 to 6 \n" << std::endl;
                return;
            }
            else {
                linuxPriotiryClass = priorityClass;
                if(linuxPriotiryClass == 0) {
                    linuxNicenessValue = 19;
                    reniceTask(linuxNicenessValue, PID);
                }
                else if(linuxPriorityClass == 1) {
                    linuxNicenessValue = 14;
                    reniceTask(linuxNicenessValue, PID);
                }
                else if(linuxPriorityClass == 2) {
                    linuxNicenessValue = 9;
                    reniceTask(linuxNicenessValue, PID);
                }
                else if(linuxPriorityClass == 3) {
                    linuxNicenessValue = 4;
                    reniceTask(linuxNicenessValue, PID);
                }
                else if(linuxPriorityClass == 4) {
                    linuxNicenessValue = -1;
                    reniceTask(linuxNicenessValue, PID);
                }
                else if(linuxPriorityClass == 5) {
                    linuxNicenessValue = -6;
                    reniceTask(linuxNicenessValue, PID);
                }
                else (linuxPriorityClass == 6) {
                    linuxNicenessValue = -11;
                    reniceTask(linuxNicenessValue, PID);
                }
            }
        }
        
        //set nodePriority of this task
        void setNodePriority(int np) {
            if (np < 1 || np > 9) {
                std::cout << "[LoadBalancingTask Class]: not a valid node priority, range form 1 to 9 \n" << std::endl;
                return;
            }
            else {
                nodePriority = np;
            }
        }
        
        //for reniceing tasks the program must be called with root rights
        void reniceTask(int nicenessInt, int PID) {
            if(nicenessInt < -20 || nicenessInt > 19) {
                std::cout << "[LoadBalancingTask Class]: not a valid nice value \n" << std::endl;
                return;
            }
            char nicenessChar[4];
            if(nicenessInt < -9) {
                nicenessChar[0] = '-';
                nicenessChar[1] = (char)((int)((-nicenessInt)/10)+48);
                nicenessChar[2] = (char)((int)((-nicenessInt)%10)+48);
                nicenessChar[3] = '\0';
            }
            else if (nicenessInt >= -9 && nicenessInt < 0) {
                nicenessChar[0] = '-';
                nicenessChar[1] = (char)((int)((-nicenessInt)%10)+48);
                nicenessChar[2] = '\0';
            }
            else if (nicenessInt >= 0 && nicenessInt < 10) {
                nicenessChar[0] = (char)(nicenessInt+48);
                nicenessChar[1] = '\0';
            }
            else if (nicenessInt >= 10) {
                nicenessChar[0] = (char)((int)((nicenessInt)/10)+48);
                nicenessChar[1] = (char)((int)((nicenessInt)%10)+48);
                nicenessChar[2] = '\0';
            }
            else {
                std::cout << "[LoadBalancingTask Class]: niceness not recognised \n" << std::endl;
            }
            
            if(PID < 2) {
                std::cout << "[LoadBalancingTask Class]: not a valid PID \n" << std:endl;
            }
            int length = snprintf(NULL, 0, "%d", PID) + 1;
            char PIDChar[length];
            snprintf(PIDChar, length, "%d", PID);
            
            char command[30];
            strcpy(command, "sudo renice ");
            strcat(command, nicenessChar);
            strcat(command, " -p ");
            strcat(command, PIDChar);
            
            system(command);
        }
        
        
        
        
    public:
        
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
        
        //increase the priority of this task, call this function
        //automated chacking for priority class
        //return 1 on success -1 if failure occured
        int increaseLinuxPriority(void) {
            //check if priority can be increased
            bool increasePossible = 0;
            if(priorityClass == 0) {
                if(linuxNicenessValue > 15) {
                    increasePossible = 1;
                }
            }
            else if (priorityClass == 1) {
                if(linuxNicenessValue > 10) {
                    increasePossible = 1;
                }
            }
            else if (priorityClass == 2) {
                if(linuxNicenessValue > 5) {
                    increasePossible = 1;
                }
            }
            else if (priorityClass == 3) {
                if(linuxNicenessValue > 0) {
                    increasePossible = 1;
                }
            }
            else if (priorityClass == 4) {
                if(linuxNicenessValue > -5) {
                    increasePossible = 1;
                }
            }
            else if (priorityClass == 5) {
                if(linuxNicenessValue > -10) {
                    increasePossible = 1;
                }
            }
            else if (priorityClass == 6) {
                if(linuxNicenessValue > -15) {
                    increasePossible = 1;
                }
            }
            //if not possible - std::cout
            //if not possible - return -1
            else {
                std::cout << "[LoadBalancingTask Class]: increase priority is not possible - priority reached max of this class \n" << std::endl;
                return -1;
            }
            //if possible - lower linuxNicenessValue
            linuxNicenessValue = linuxNicenessValue - 1;
            //if possible - call reniceTask
            reniceTask(linuxNicenessValue, PID);
            //if possible - return 1
			return 1;
            
        }
        
        //constructor
        //default constructor, don't use this
        LoadBalancingTask() {
            internal = -1;
            PID = -10;
            taskNumber = -1;
            linuxNicenessValue = -1;
            linuxPriotiryClass = -1;
            nodePriority = -1;
        }
        
        //use this constructor instead
        LoadBalancingTask(int task, int place, int linuxPID, int np, int pClass) {
            setTaskNumber(task);
            setInternal(place);
            setPID(linuxPID);
            setNodePriority(np);
            setlinuxPriorityClass(pClass);
            setTimeStamp();
        }
        
        //destructor, return the livetime of this object in seconds - does not work
        ~LoadBalancingTask() {
            
        }
};


float readNP(int taskNumber);
float readLoad(int minPriority);
float readLoadIM(int minPriority);
bool amIInternally (void);




#endif