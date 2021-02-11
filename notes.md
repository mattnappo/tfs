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
 * Check for filename length safety
 * Write a better `new_file` method to read a file from disk better
 * Better error handling

# Other features:
 * ~~Serialization~~
 * Algorithmic file offset calculation
 * Networking
 * Encryption
 * Add blocks to memory

# Network features
The server will host multiple filesystems. The client can request:
 * To get a file
 * To put a file
 * To make a new fs
 * To get the entire fs
 * To get all fs ids

