# CP - Operating System Simulator (Phase 2)

## Overview
This project implements **Phase 2** of an Operating System Simulator, which simulates CPU and memory management for executing user programs. It includes features like page table management, virtual-to-physical address translation, memory allocation, and job execution with error handling.

## Project Structure
```
cp/
├── Phase2.c              # Main implementation of the OS simulator
├── phase2input.txt       # Input file containing job specifications and data
├── phase2output.txt      # Output file with execution results
└── README.md             # This file
```

## Key Features

### Memory Management
- **Main Memory**: 300×4 byte blocks for storing program instructions and data
- **Page Table**: Manages virtual-to-physical address translation
- **General Purpose Register**: 4-byte register for data operations

### Process Control Block (PCB)
Tracks job execution information:
- `JOBID`: Unique job identifier
- `TTL`: Total Time Limit
- `TLL`: Total Line Limit
- `TTC`: Total Time Counter
- `LLC`: Line Limit Counter

### Core Operations

#### 1. **Load Phase**
- Loads job specifications and data from input file
- Initializes page tables for each job
- Sets up memory allocation

#### 2. **Execution Phase**
- Executes user program instructions
- Handles virtual memory addressing
- Performs read/write operations

#### 3. **Instruction Set**
Common instructions used:
- `GD`: Get Data (read input)
- `PD`: Put Data (write output)
- `LR`: Load Register
- `SR`: Store Register
- `CR`: Compare Register
- `BT`: Branch if True
- `H`: Halt

#### 4. **Error Handling**
The simulator handles various error conditions:
- **Line Limit Exceeded**: When output lines exceed limit (Terminate 2)
- **Out Of Data Error**: When input data exhausted prematurely (Terminate 1)
- **Invalid Page Fault**: When accessing invalid memory page (Terminate 6)
- **Out Of Memory**: When memory allocation fails

### System Tables and Registers
- `M[][]`: Main memory (300×4 bytes)
- `R[]`: General purpose register (4 bytes)
- `IR[]`: Instruction register (4 bytes)
- `IC`: Instruction counter
- `SI`: System interrupt (for I/O operations)
- `C`: Toggle switch
- `PTR`: Page table register
- `EM`: Error module
- `TI`, `PI`: Time and page interrupt flags

## Input File Format
```
$AMJ<JOBID><TTL><TLL><Line>
<Instructions>
$DTA
<Data lines>
$END<JOBID>
```

### Example:
```
$AMJ020200250004
GD20PD20LR20SR30SR31PD30SR40SR41SR42PD40
SR50SR51PD50SR60PD60H  
$DTA
*
$END0202
```

## Output
The output includes:
- Execution results (data processed)
- Job statistics (Job ID, IC, IR)
- Error messages and termination reasons
- Time and line limit counters

## Execution

### Run the Program
```bash
./Phase2.exe
```

The program reads from `phase2input.txt` and writes results to `phase2output.txt`.

## Sample Execution

### Input (phase2input.txt)
Jobs with instructions and data to be processed.

### Output (phase2output.txt)
- Execution traces
- Job completion status
- Error messages
- Performance counters

## Main Functions

| Function | Purpose |
|----------|---------|
| `Load()` | Load jobs and initialize memory |
| `INIT()` | Initialize system state |
| `INIT_PT()` | Initialize page table |
| `StartExecution()` | Begin program execution |
| `ExecuteUserProgram()` | Execute individual instructions |
| `address()` | Virtual to physical address translation |
| `Read()` | Read data from input |
| `Write()` | Write data to output |
| `Terminate()` | Handle job termination |
| `MOS()` | Handle OS operations |

## Error Codes
- **0**: Normal termination
- **1**: Out of Data error
- **2**: Line Limit Exceeded
- **6**: Invalid Page Fault

## Requirements
- **Compiler**: GCC or compatible C compiler
- **OS**: Windows, Linux, or macOS
- **Input File**: `phase2input.txt` in the same directory

## Notes
- The simulator processes jobs sequentially
- Virtual addresses are translated to physical addresses using page tables
- Error conditions cause immediate job termination with status reporting


## License
Operating Systems Simulation Course Project  
