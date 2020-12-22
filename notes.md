# Some random notes

# Todo:
 * Check for filename length safety
 * Change all chars to uint8\_ts where necessary
 * Write the algorithm to find free memory
 * Write a CLI
 * Better error handling

# Other features:
 * Serialization
 * Encryption
 * Add blocks to memory
 * Network it
 * Filepaths?

# Small things:
 * For dumping memory, add a feature to dump to a buffer

# Note:
It looks like I got string operations confused. There really was no bug--I was trying to print out a non-null terminated pointer to a char (from a memory read). Be careful with this, and know when to use strlen, strcpy, and memcpy.
