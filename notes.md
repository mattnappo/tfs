# Some random notes

# Cool features to add

## Multiple FSs
 - Server will create a folder on disk called fsdb/
 - It will contain all filesystems on it
 - Will use the FSID in the req to lookup the right fs
 - Will need to write data structure (table?) to hold multiple filesystems

## Authentication
 - Hash password on client side, send to server
 - Server accepts or doesn't accept hash, responds with token
 - Server stores tokens in some in-memory database
 - Client must then send the token in every request

## Other features
 * CLI
 * Logging?
 * Algorithmic file offset calculation
 * Add blocks to vdisk

# Non-networking todo:
 * Add functionality for deleting files
 * Write a better `new_file` method to read a file from disk better

# Network features todo
The client can request to:
 * ~~get an entire fs~~
 * ~~get a file~~
 * ~~put a file~~
 * make a new fs
 * get all fs ids

# The plan
1. Global server state
2. Multiple filesystems
3. Authorization
4. CLI
