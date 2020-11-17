# Some random notes

# Todo:
 * Check for filename length safety
 * Write the algorithm to find free memory

# Other features:
 * Serialization
 * Encryption
 * Add blocks to memory
 * Network it
 * Filepaths?

# Small things:
 * For dumping memory, add a feature to dump to a buffer

# Bug hunting:
Hmmm.... no memory leaks or memory errors in the ftable test. Possible idea: play around with the function that reads from disk. Is it possible that the bug is in memory.read? Probably not... no errors when running w/o `interface file` creation in `struct file get_file(struct fs *, char[])`

Update: Ohhh, it looks like I got string operations confused. There really was no bug--I was trying to print out a non-null terminated pointer to a char (from a memory read). Be careful with this, Matt, and know when to use strlen, strcpy, and memcpy.
