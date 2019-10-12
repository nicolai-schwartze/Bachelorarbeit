

void writeCPULoadToCSV(float cpuload, _Bool *outsourcingTasks) {
    FILE *filePointer = NULL;
    filePointer = fopen("cpuload.csv", "a");
    fprintf(filePointer, "%f; %d; %d; %d; %d; %d; %d; %d;\n", cpuload, outsourcingTasks[0], outsourcingTasks[1], outsourcingTasks[2], outsourcingTasks[3], outsourcingTasks[4], outsourcingTasks[5], outsourcingTasks[6]);
    fclose(filePointer);
    filePointer = NULL;
}
 