# Some random notes

# Currently:
1. Allow for multiple FSs
 - Server will create a folder on disk called fsdb/
 - It will contain all filesystems on it
 - Will use the FSID in the req to lookup the right fs
 - Will need to write data structure (table?) to hold multiple filesystems
2. Unified type for requests AND responses

# Non-networking todo:
 * Add functionality for deleting files
 * Write a better `new_file` method to read a file from disk better

# Other features:
 * Algorithmic file offset calculation
 * Encryption
 * Add blocks to vdisk

# Network features
The server will host multiple filesystems. The client can request:
 * To get a file: __IN PROGRESS CURRENTLY__
 * ~~To get an entire fs~~
 * To put a file
 * To make a new fs
 * To get all fs ids


Last thing I was doing: write line 123 of server.c
