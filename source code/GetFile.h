/// Opens a dialog asking for a file and stores the result in the given buffer.
///
/// out_buffer: The buffer to write the filename to. 
///             The result will be a null-terminated string. 
///             If no file is selected, the result will be an empty string.
/// bufferlen:  The maximum number of characters to write
void GetFile(char* out_buffer, size_t bufferlen);