#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

char M[300][4]; // Main memory
char R[4];      // General Purpose Register
int IC, SI;     // Instruction Counter, SI for read write and terminate
char IR[4];     // Current Instruction
// char IR[5];
int C; // Toggle switch
int PTR;
int EM;
int TI, PI;
int pte;
struct PCB // process control block
{
    int JOBID, TTL, TLL, TTC, LLC;
};
struct PCB pcb;
int job_term;
int lastlineend;
int totalinst;
int venu;
char buffer[41];      // Temporary buffer to load data from input file and store it in main memory
FILE *Input, *Output; // File pointers

int main();
void Load();
void INIT();
void bufferclear();
void INIT_PT(int PTR);
int address_storing(int VA);
void getstoreloc(int pteloc);
void StartExecution();
void ExecuteUserProgram();
int address(int VA);
void Read();
void Write();
void MOS();
void Terminate(int caes);
void Terminatee(int caes1, int caes2);

void bufferclear() // Used to clear the buffer before using it again.
{
    for (int i = 0; i < 41; i++) // since 41 spaces long buffer
    {
        buffer[i] = '\0'; // end line character
    }
}

int random()
{
    int random_number = rand() % 30;
    return random_number;
}

void getstoreloc(int pteloc)
{
    int def;
repeat:
    def = random() * 10;
    if (def == PTR)
        goto repeat;
    for (int i = 0; i < 10; i++)
    {
        if (M[PTR + i][0] == '*')
            continue;
        int num = ((M[PTR + i][0] - '0') * 1000 + (M[PTR + i][1] - '0') * 100 + (M[PTR + i][2] - '0') * 10 + (M[PTR + i][3] - '0')) * 10;
        if (num == def)
        {
            goto repeat;
        }
    }
    int val = def / 10;
    M[pteloc][0] = (val / 1000) % 10 + '0';
    M[pteloc][1] = (val / 100) % 10 + '0';
    M[pteloc][2] = (val / 10) % 10 + '0';
    M[pteloc][3] = val % 10 + '0';
}

int address(int VA)
{
    int RA;
    pte = PTR + (VA / 10);
    if (M[pte][0] == '*')
    {
        PI = 3;
        return -1;
    }
    RA = (((M[pte][0] - '0') * 1000 + (M[pte][1] - '0') * 100 + (M[pte][2] - '0') * 10 + (M[pte][3] - '0')) * 10) + (VA % 10);
    return RA;
}

int address_storing(int VA)
{
    int RA;
    pte = PTR + (  VA / 10);
    if (M[pte][0] == '*')
    {
        getstoreloc(pte);
    }
    RA = (((M[pte][0] - '0') * 1000 + (M[pte][1] - '0') * 100 + (M[pte][2] - '0') * 10 + (M[pte][3] - '0')) * 10) + (VA % 10);
    return RA;
}

void Read()
{
    // int ghi = Getloc(); // fetch address where to store the data defined by user
    int loc = address(((IR[2] - '0') * 10) + (IR[3] - '0'));
    if (loc == -1)
    {
        // printf("HereGD");
        MOS();
        if (job_term)
            return;
        loc = address(((IR[2] - '0') * 10) + (IR[3] - '0'));
    }
    int k = 0; // for looping and storing the value from input file
    bufferclear();

    fgets(buffer, 41, Input); // gets first 41 characters

    if (strncmp(buffer, "$END", 4) == 0)
    {
        // printf("Yayy");
        pcb.TTC -= 1;
        Terminate(1);
        lastlineend = 1;
        if (job_term)
            return;
    }

    while (k < 41 && buffer[k] != '\0') // until end of line is detected
    {
        for (int j = 0; j < 4; j++) // since every block has 4 bytes
        {
            if (buffer[k] == '\n')
            {
                k++;
                continue;
            }
            M[loc][j] = buffer[k]; // storing the 4 bytes at current word
            k++;                   // for byte location change
        }
        loc++; // jumping onto the next word location
    }
}

void Write()
{
    Output = fopen("phase2output.txt", "a"); // No overwriting, continue from last left location
    if (Output == NULL)                      // Check for output file
    {
        printf("Output file not found.\n");
        return;
    }
    int loc = address(((IR[2] - '0') * 10) + (IR[3] - '0')); // Location from where to print data

    if (loc == -1)
    {
        // printf("HereGD");
        MOS();
        fclose(Output);
        if (job_term)
            return;
    }

    pcb.LLC += 1;
    if (pcb.TLL < pcb.LLC && TI != 2)
    {
        // printf("Yoss");
        pcb.LLC -= 1;
        Terminate(2);
    }

    for (int i = loc; i < loc + 10; i++) // for 1 block printing
    {
        for (int k = 0; k < 4; k++) // for 4 bytes
        {
            if (M[i][k] != '\0')
            {
                fprintf(Output, "%c", M[i][k]);
            }
            else
            {
                fprintf(Output, " ", M[i][k]);
            }
        }
    }
    fprintf(Output, "\n"); // Next line in file
    fclose(Output);        // File close
}

void Terminate(int caes)
{
    Output = fopen("phase2output.txt", "a"); // File opened in write mode, writing from current position, no overwrite
    if (Output == NULL)                      // File not found
    {
        printf("Output file not found.\n");
        return;
    }
    fprintf(Output, "JOB ID: %04d\n", pcb.JOBID);
    switch (caes)
    {
    case 0:
        if (venu == 0)
        {
            fprintf(Output, "No Error : Terminate(%d) [With TI = 2]\n", caes);
        }
        else
        {
            fprintf(Output, "No Error : Terminate(%d)\n", caes);
        }
        break;

    case 1:
        fprintf(Output, "Out Of Data Error : Terminate(%d)\n", caes);
        break;

    case 2:
        fprintf(Output, "Line Limit Exceeded Error : Terminate(%d)\n", caes);
        break;

    case 3:
        fprintf(Output, "Time Limit Exceeded Error : Terminate(%d)\n", caes);
        break;

    case 4:
        fprintf(Output, "Operation Code Error : Terminate(%d)\n", caes);
        break;

    case 5:
        fprintf(Output, "Operand Error : Terminate(%d)\n", caes);
        break;

    case 6:
        fprintf(Output, "Invalid Page Fault Error : Terminate(%d)\n", caes);
        break;

    default:
        break;
    }
    // fprintf(Output, "Total IC: %d , ", totalinst);
    //
    fprintf(Output, "IC: %d\n", IC);
    fprintf(Output, "IR: %s\n", IR);
    //fprintf(Output, "PTR: %04d , ", PTR);
    //fprintf(Output, "TTL: %04d , ", pcb.TTL);
    //fprintf(Output, "TLL: %04d , ", pcb.TLL);
    fprintf(Output, "TTC: %04d\n", pcb.TTC);
    fprintf(Output, "LLC: %04d\n", pcb.LLC);
    //fprintf(Output, "SI: %d , ", SI);
    //fprintf(Output, "PI: %d , ", PI);
    //fprintf(Output, "TI: %d\n", TI);
    //fprintf(Output, "End of Job %04d\n", pcb.JOBID);
    fprintf(Output, "\n"); // Leave space after program done
    fclose(Output);        // File closed

    job_term = 1;
}

void Terminatee(int caes1, int caes2)
{
    Output = fopen("phase2output.txt", "a"); // File opened in write mode, writing from current position, no overwrite
    if (Output == NULL)                      // File not found
    {
        printf("Output file not found.\n");
        return;
    }
    fprintf(Output, "JOB ID: %04d\n", pcb.JOBID);
    if (caes1 == 3 && caes2 == 4)
    {
        fprintf(Output, "Time Limit Exceeded Error : Terminate(%d)\n", caes1);
        fprintf(Output, "Operation Code Error : Terminate(%d)\n", caes2);
    }
    if (caes1 == 3 && caes2 == 5)
    {
        fprintf(Output, "Time Limit Exceeded Error : Terminate(%d)\n", caes1);
        fprintf(Output, "Operand Error : Terminate(%d)\n", caes2);
    }
    // fprintf(Output, "Total IC: %d , ", totalinst);
    //fprintf(Output, "JOB ID: %04d\n", pcb.JOBID);
    fprintf(Output, "IC: %d\n", IC);
    fprintf(Output, "IR: %s\n", IR);
    //fprintf(Output, "PTR: %04d , ", PTR);
    //fprintf(Output, "TTL: %04d , ", pcb.TTL);
    //fprintf(Output, "TLL: %04d , ", pcb.TLL);
    fprintf(Output, "TTC: %04d\n", pcb.TTC);
    fprintf(Output, "LLC: %04d\n", pcb.LLC);
    //fprintf(Output, "End of Job %04d\n", pcb.JOBID);
    fprintf(Output, "\n"); // Leave space after program done
    fclose(Output);        // File closed

    job_term = 1;
}

void MOS()
{
    if (TI == 0 && PI == 1)
    {
        // printf("I am here");
        Terminate(4);
    }
    else if (TI == 0 && PI == 2)
    {
        Terminate(5);
    }
    else if (TI == 0 && PI == 3)
    {
        if (IR[0] == 'G' && IR[1] == 'D' || IR[0] == 'S' && IR[1] == 'R')
        {
            getstoreloc(pte);
            PI = 0;
        }
        else
        {
            pcb.TTC -= 1;
            Terminate(6);
        }
    }
    else if (TI == 2 && PI == 1)
    {
        Terminatee(3, 4);
    }
    else if (TI == 2 && PI == 2)
    {
        Terminatee(3, 5);
    }
    else if (TI == 2 && PI == 3)
    {
        Terminate(3);
    }
    else if (TI == 0 && SI == 1)
    {
        Read();
    }
    else if (TI == 0 && SI == 2)
    {
        Write();
    }
    else if (TI == 0 && SI == 3)
    {
        Terminate(0);
    }
    else if (TI == 2 && SI == 1)
    {
        pcb.TTC -= 1;
        Terminate(3);
    }
    else if (TI == 2 && SI == 2)
    {
        Write();
        Terminate(3);
    }
    else if (TI == 2 && SI == 3)
    {
        venu = 0;
        Terminate(0);
    }
}

void ExecuteUserProgram()
{
    while (!job_term) // Continue to loop until said otherwise
    {
        int loc = address(IC);
        // printf("loc:%d,IC:%d\n", loc, IC);
        if (loc == -1)
        {
            printf("Hereloc");
            MOS();
            if (job_term)
                break;
        }
        if (PI != 0)
        {
            // printf(" - Wajh mai hu");
            //SI = 0;
            MOS();
            if (job_term)
                break;
        }
        for (int i = 0; i < 4; i++) // since 4 bytes
        {
            IR[i] = M[loc][i]; // current value of instruction counter
        }

        if (IR[0] != 'H' && !(isdigit(IR[2]) && isdigit(IR[3])))
        {
            // printf("Heyoo");
            PI = 2;
            IC++;
            pcb.TTC += 1;
            if (pcb.TTL < pcb.TTC)
            {
                TI = 2;
            }
            pcb.TTC -= 1;
            MOS();
            if (job_term)
                break;
        }
        IC++; // point to next instruction

        if (IR[0] == 'G' && IR[1] == 'D') // GD - Get data
        {
            pcb.TTC += 1;
            SI = 1;
            if (pcb.TTL < pcb.TTC)
            {
                TI = 2;
                MOS();
                break;
            }
            MOS();
            if (job_term)
                break;
        }
        else if (IR[0] == 'P' && IR[1] == 'D') // PD - Print data
        {
            pcb.TTC += 1;
            SI = 2;
            if (pcb.TTL < pcb.TTC)
            {
                TI = 2;
                MOS();
                break;
            }
            MOS();
            if (job_term)
                break;
        }
        else if (IR[0] == 'H') // H - Halt
        {
            pcb.TTC += 1;
            if (pcb.TTL < pcb.TTC)
            {
                TI = 2;
            }
            SI = 3;
            MOS();
            break; // to get out of continuation of while loop
        }
        else if (IR[0] == 'L' && IR[1] == 'R') // LR - Load Register
        {
            pcb.TTC += 1;
            if (pcb.TTL < pcb.TTC)
            {
                SI = 1;
                TI = 2;
                MOS();
                break;
            }
            int loc = address(((IR[2] - '0') * 10) + (IR[3] - '0'));
            if (loc == -1)
            {
                // printf("HereGD");
                MOS();
                if (job_term)
                    break;
            }
            for (int i = 0; i < 4; i++)
            {
                R[i] = M[loc][i]; // copy 4 bytes to register
            }
        }
        else if (IR[0] == 'S' && IR[1] == 'R') // SR - Store Register value back
        {
            pcb.TTC += 1;
            if (pcb.TTL < pcb.TTC)
            {
                SI = 1;
                TI = 2;
                MOS();
                break;
            }
            int loc = address(((IR[2] - '0') * 10) + (IR[3] - '0'));
            if (loc == -1)
            {
                MOS();
                if (job_term)
                    break;
                loc = address(((IR[2] - '0') * 10) + (IR[3] - '0'));
            }
            for (int i = 0; i < 4; i++)
            {
                M[loc][i] = R[i]; // Copy data from register to memeory location
            }
        }
        else if (IR[0] == 'C' && IR[1] == 'R') // CR - Compare
        {
            pcb.TTC += 1;
            if (pcb.TTL < pcb.TTC)
            {
                SI = 1;
                TI = 2;
                MOS();
                break;
            }
            int loc = address(((IR[2] - '0') * 10) + (IR[3] - '0'));
            if (loc == -1)
            {
                // printf("HereGD");
                MOS();
                if (job_term)
                    break;
            }
            C = 1; // Toggle set to TRUE, as if 1st byte match not necessary all will in comparison below
            for (int i = 0; i < 4; i++)
            {
                if (M[loc][i] != R[i]) // Comparing data location with data stored in register
                {
                    C = 0; // Toggle False
                    break; // Jump out of loop
                }
            }
        }
        else if (IR[0] == 'B' && IR[1] == 'T') // BT - Branch
        {
            pcb.TTC += 1;
            if (pcb.TTL < pcb.TTC)
            {
                SI = 1;
                TI = 2;
                MOS();
                break;
            }
            if (C) // If toggle true
            {
                int loc = ((IR[2] - '0') * 10) + (IR[3] - '0');
                IC = loc; // Store location to branch in IC
            }
        }
        else
        {
            // printf(" - Me too - ");
            SI = 0;
            PI = 1;
            pcb.TTC += 1;
            if (pcb.TTL < pcb.TTC)
            {
                TI = 2;
            }
            pcb.TTC -= 1;
            MOS();
            if (job_term)
                break;
        }

        if (pcb.TTL < pcb.TTC)
        {
            TI = 2;
            MOS();
            if (job_term)
                break;
        }
    }
}

void StartExecution()
{
    IC = 0; // Counter set to 0, as it was pointing to the first empty location of memeory
    ExecuteUserProgram();
}

void INIT()
{
    for (int i = 0; i < 300; i++) // Since 300 blocks of 4 byte each
    {
        for (int j = 0; j < 4; j++)
        {
            M[i][j] = '\0'; // Initialised to newline character
        }
    }

    for (int i = 0; i < 4; i++) // since 4 bytes long
    {
        R[i] = ' ';  // Stored blank space
        IR[i] = ' '; // Stored blank space
    }
    // IR[4] = '\0';

    IC = 0; // Instruction counter
    SI = 3;
    C = 0; // Toggle set to false
    TI = 0;
    PI = 0;
    EM = 0;
    PTR = 0;
    char temp1[5], temp2[5], temp3[5];
    for (int l = 0; l < 4; l++)
    {
        temp1[l] = buffer[l + 4];
        temp2[l] = buffer[l + 8];
        temp3[l] = buffer[l + 12];
    }
    temp1[4] = '\0';
    temp2[4] = '\0';
    temp3[4] = '\0';
    pcb.JOBID = atoi(temp1);
    pcb.TTL = atoi(temp2);
    pcb.TLL = atoi(temp3);
    pcb.LLC = 0;
    pcb.TTC = 0;
    job_term = 0;
    lastlineend = 0;
    totalinst = 0;
    bufferclear();
    venu=1;
}

void INIT_PT(int PTR)
{
    // int abc = PTR + 10;
    for (int i = PTR; i < (PTR + 10); i++)
    {
        for (int j = 0; j < 4; j++)
        {
            M[i][j] = '*';
        }
    }
}

void Load()
{
    if (Input == NULL) // Checks for empty file
    {
        printf("Empty File\n");
        return;
    }

    int job = 0; // Used to indicate whether a job is in progress

    while (fgets(buffer, 41, Input) != NULL) // Copies 1 whole line from input file, upto length 41 only into buffer
    {
        if (strncmp(buffer, "$AMJ", 4) == 0)
        {
            if (job) // if a job is in progress
            {
                // Terminate(0);
            }
            INIT(); // Initialise as new job started
            // int xyz = random();
            PTR = random() * 10;
            INIT_PT(PTR);
            job = 1; // Set to 1, indicate a job has started
        }
        else if (strncmp(buffer, "$DTA", 4) == 0)
        {
            if (!job) // No job found
            {
                printf("DTA found before AMJ\n");
                return;
            }
            bufferclear(); // Clearing buffer before fetching data from input file
            StartExecution();
            if (lastlineend != 0)
            {
                for (int i = 0; i < 300; i++) // Printing all the data for 30 blocks i.e 300 words.
                {
                    printf("M[%d]: %c%c%c%c\n", i, M[i][0], M[i][1], M[i][2], M[i][3]);
                }
                printf("\n\n");

              
                job = 0;
            }
        }
        else if (strncmp(buffer, "$END", 4) == 0)
        {
            if (!job) // If no job in progress
            {
                printf("END found before AMJ\n");
                return;
            }
            for (int i = 0; i < 300; i++) // Printing all the data for 30 blocks i.e 300 words.
            {
                printf("M[%d]: %c%c%c%c\n", i, M[i][0], M[i][1], M[i][2], M[i][3]);
            }
            printf("\n\n");

            job = 0;
        }
        else if (job_term != 1)
        {
            int i = address_storing(IC); // current instruction position
            // printf("Here %d %d\n",IC,i);
            int j = 0;                                               // for loop iteration
            while (j < 41 && buffer[j] != '\0' && buffer[j] != '\n') // \n coz it was jumping a line in output print of memory, \0 showing end of line
            {
                for (int k = 0; k < 4; k++) // for 4 bytes
                {
                    if (buffer[j] == 'H' && buffer[j] != '\n') // case for halt as only 1 byte instruction
                    {
                        M[i][k] = 'H';
                        j++;
                        break;
                    }
                    M[i][k] = buffer[j++];
                }
                i++;
                IC++;
            }
            totalinst = IC;
        }
        bufferclear();
    }

    if (job)
    {
        Terminate(0); // Terminate when end of code for job is reached, not end of data or END but end of code part only
    }
}

int main()
{
    srand(time(NULL));
    Input = fopen("phase2input.txt", "r");   // Opening the input file in read mode
    Output = fopen("phase2output.txt", "w"); // Opening the output file in write mode

    if (Input == NULL) // Check for input file
    {
        printf("File Not Found\n");
        return 1;
    }
    else
    {
        printf("Input file opened successfully\n");
    }

    Load();

    fclose(Input);  // Input file closed
    fclose(Output); // Output file closed
    return 0;
}