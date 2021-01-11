# Some random notes

# Currently:
1. Finish writing protocol.c
2. Write client
2. Write client_test

# Todo:
 * Check for filename length safety
 * Write a better `new_file` method to read a file from disk better
 * Write fs.c: `print_file`
 * Write the algorithm to find free memory
 * Better error handling

# Other features:
 * ~~Serialization~~
 * Networked CLI
 * Encryption
 * Add blocks to memory


# Network features
The server will host a filesystem (or perhaps multiple filesystems). The client can request:
 * To get a file
 * To put a file
 * To get the entire fs
 * To get fs id

