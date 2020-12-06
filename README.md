# [ICSOS-VM-Manager](https://github.com/imliuyzh/ICSOS-VM-Manager)
A virtual memory (VM) system simulation with segmentation and paging using C++11. The system manages the necessary segment and page tables (PTs) in an imitiated main memory. It accepts virtual addresses (VAs) and translates them into physical addresses (PAs) according to the current contents of the segment and PTs.

## Background
### Segmentation with Paging
A logical address space consists of a sequence of imaginary memory locations mapped onto PM locations. When the size of the logical address space is allowed to exceed the size of the actual physical space, the space is called a virtual memory.
 
With pure segmentation, the logical address space of a process is divided into variable-size blocks, each typically corresponding to a logical component of the process. Each block is mapped into a contiguous portion of PM. A segment table (ST) is used to keep track of the starting addresses of all segments.
 
Each VA is divided into 2 components, ```(s, w)```, where ```s``` is the segment number (an offset into the segment table) and ```w``` is the offset within the segment.
 
With pure paging, the logical address space of a process is divided into fixed-size blocks, called pages. The PA space is divided into fixed-size blocks called page frames. The frame size and the page size must be identical and thus any page may be mapped into any available frame. A PT is used to keep track which page is in which frame.
 
Each VA is divided into 2 components, ```(p, w)```, where ```p``` is the page number (an offset into the PT) and ```w``` is the offset within the page.
 
To gain the advantages of both approaches, segmentation can be combined with paging in that each segment is divided into fixed-size pages, which do not have to be contiguous in PM. Each entry of the ST points to a PT corresponding to one segment, and each entry of the PT points to one page comprising the address space of the process. Each VA is then divided into 3 components, ```(s, p, w)```, where ```s``` is the segment number (offset into the segment table), ```p``` is the page number (offset into the PT), and ```w``` is the offset within the page.

### Address Translation
A VA is a nonnegative integer. The number of bits used to represent the VA determines the size of the VA space. For example, with a VA of 32 bits, ```2^32``` addresses can be created. The number of bits used to represent ```s```, ```p```, ```w```, and ```pw``` determines the size of the segment table, the PT, each page, and the offset into the segment respectively.
 
A PA is also a nonnegative integer and the number of bits used to represent the PA determines the size of the PM.
 
The first step to translate VAs is to break the VA into ```s```, ```p```, ```w```, and ```pw```, each becomes a separate integer. The segment number ```s``` is used as an offset into the ST to find the corresponding PT. The page number ```p``` is used as an offset into the PT to find the corresponding page. The offset ```w``` is added to the starting address of the page to form the PA. ```pw``` is the offset into the segment s and must not exceed the segment size.

<img src="https://i.ibb.co/jwx3WSH/vmtlb1v2-1.png" width=500 />

### VM with Demand Paging
#### Demand Paging
If the size of the VM exceeds the size of the PM, then not all pages can be present in PM at all times, but must be loaded from a disk as needed. When a nonresident page is needed and no free frame is available, one of the resident pages must be removed and replaced by the new page. Thus, the PT of a given segment may not be resident when a VA is being translated.
 
To keep track of which pages of a segment s are currently resident, an additional bit, called the present bit, is associated with each entry in the PT. Similarly, each entry of an ST contains a present bit to record the presence or absence of the corresponding PT. If the present bit is true, then the entry contains the frame number of the page or PT. If the present bit is false, then the entry contains the location of the page or PT on the disk.
 
Referencing a nonresident page or PT is called a page fault, which the memory manager must resolve by locating the missing page or PT on the disk, allocating a page frame, and loading the missing page or PT into the frame

##### A Paging Disk
The paging disk is emulated as a two-dimensional integer array, ```D[B][512]```, where ```B``` is the number of blocks and 512 is the block size (equal to the page size). The disk may only be accessed one block at a time. The function ```readBlock(b, m)``` copies the entire block ```D[b]``` into the PM frame starting at location ```PM[m]```. For this project, the paging disk is an integer array ```D[1024][512]```.

##### List of Free Frames
Since blocks may need to be moved to the PM from the paging disk, the memory manager must use a linked list to keep track of which memory frames are free. In addition, assume the PM will always have a sufficient number of free frames available so that no page replacement algorithm is needed.

#### VM Specification
+ Memory is word-addressable, where each word is an integer
+ Each entry of the ST consists of 2 integer components:
    + The size of the segment s (number of words)
    + The number of the page frame holding the PT of segment s
    + If the segment does not exist, then both fields contain a 0
+ Each entry in a PT contains the number of the frame holding the corresponding page
    + If the page does not exist, then the entry contains a 0
+ A VA is an integer of 32 bits, divided into the 3 components: s, p, and w (the leading 5 bits of the VA are unused)
    + The size of each page is 9 bits (512 words/integers)
    + The size of each PT is also 9 bits
    + The size of the ST is 9 bits
+ To indicate that a PT is currently not resident in PM, the corresponding ST entry contains a negative number, ```−b```, where the absolute value ```|−b| = b``` is the block number on the paging disk that contains the PT
+ To indicate that a page is currently not resident in PM, the corresponding PT entry contains a negative number, ```−b```, where the absolute value ```|−b| = b``` is the block number on the paging disk that contains the page
+ The sign bit in each ST or PT entry represents the present bit

#### PM Specification
+ PM is represented as an array of 524,288 integers
    + Divided into 1024 frames of 512 words each. Consequently, the size of a PA is 19 bits
+ Each entry of the ST requires 2 integers. Thus, the ST needs 2 frames and resides always in frames 0 and 1

## Running the Application
This program will take an init and input file and display several translated physical addresses in a file named ```output-dp.txt```.

### Built With
Please install these applications if you want to compile the program yourself:
+ g++
+ Valgrind

### Steps
1. Clone/download this repository to your local machine
2. Navigate to the folder in your console
3. Prepare the files which contains all your operations
4. Type ```make compile``` to compile the program
5. Type ```./icsosvm {initFile} {inputFile}``` to start the program (no brackets)
    + ```{initFile}``` is the directory to your init file
    + ```{inputFile}``` is the directory to your input file
6. Type ```make clean``` to remove all traces in your device

#### Initialization of the PM (init files)
The PM is initialized from a file, which specifies the frames of all PTs, the corresponding segment lengths, and the frame numbers of all pages (ST always resides in frames 0 and 1). All pages contain zeros as the initial values of PM.
 
The file consists of 2 lines, each containing a series of integers:
```
s_1 z_1 f_1 s_2 z_2 f_2 … s_n z_n f_n
s_1 p_1 f_1 s_2 p_2 f_2 … s_m p_m f_m
```

Each triple ```s_i z_i f_i``` on line 1 means that PT of segment si resides in frame ```f_i```. and the length of segment ```s_i``` is ```z_i```. Thus, line 1 defines the ST. For example, ```8 4000 3``` means that the PT of segment 8 resides in frame 3 and the size of segment 8 is 4000. That is, the initialization sets ```PM[2*8] = PM[16] = 4000 and PM[2*8+1] = PM[17] = 3```.
 
Each triple ```s_j p_j f_j``` on line 2 means that the page ```p_j``` of segment ```s_j``` resides in frame ```f_j```. Thus, line 2 defines the PTs. For example, ```8 5 8``` means that page 5 of segment 8 resides in frame 8. That is, the initialization sets ```PM[PM[2*8+1]*512+5] = 8```.

Beware that the third component of each triple can be a positive or a negative integer. A positive integer, ```f_i```, means that the corresponding PT or page resides in frame ```f_i```. A negative integer, ```−b_i```, means that the corresponding PT or page is currently not resident and must be loaded into a free frame from block ```b_i``` of the disk.

#### Executing VA Translations (input files)
Once the PM has been initialized, the system is ready to accept VAs and to attempt to translate them into PAs. The VAs are given in a second input file, which contains a series of integers, each representing one VA. The program must read the file and attempt to translate each VA. The result of each translation, which is either a corresponding PA or -1, is to be written to an output file.

## Acknowledgments
This project is originally developed for UC Irvine's CS 143B taught by Prof. Lubomir Bic.