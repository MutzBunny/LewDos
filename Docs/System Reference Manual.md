

### Memory Manager [Memes]
The memory management system of LewDos utelizes Bitmapped Paging. Every Memory Block has a Byte allocated to it, follwing a bitmask for Status and allocation.

| Bits | Meaning                                                |
| ---- | ------------------------------------------------------ |
| 0-5  | User ID (Allows for 63 Users. User ID 0 is the Kernel) |
| 6    | Read only Flag (1 = Read only, 0 = Full access)        |
| 7    | Free Flag (1 = Free, 0 = Allocated)                    |
To give a couple of Examples:
0xFF (b1111 1111) = Block is Free
0x03 (b0000 0011) = Block is allocated to UserID 03 and Reading and wrting enabled
0x20 (b0010 0000) = Block is reseverd for Kernel space, and set to Read only