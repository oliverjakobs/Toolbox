#ifndef TB_FILE_H
#define TB_FILE_H

#include <stdio.h>

#define TB_FILE_COPY_BUFFER_SIZE    4096

char* tb_file_read(const char* path, const char* mode);
char* tb_file_read_alloc(const char* path, const char* mode, void* (*mallocf)(size_t), void (*freef)(void*));

size_t tb_file_read_buf(const char* path, const char* mode, char* buf, size_t max_len);

/* Copies a file from src_path to dst_path without dynamic memory allocations. */
size_t tb_file_copy(const char* src_path, const char* dst_path);

#endif /* !TB_FILE_H */
/*
 * -----------------------------------------------------------------------------
 * ----| IMPLEMENTATION |-------------------------------------------------------
 * -----------------------------------------------------------------------------
 */

#ifdef TB_FILE_IMPLEMENTATION


#include <stdlib.h>

char* tb_file_read(const char* path, const char* mode)
{
	return tb_file_read_alloc(path, mode, malloc, free);
}

char* tb_file_read_alloc(const char* path, const char* mode, void* (*mallocf)(size_t), void (*freef)(void*))
{
	FILE* file = fopen(path, mode);
	if (!file) return NULL;

	/* find file size */
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	rewind(file);

	char* buffer = mallocf(size + 1);
	if (buffer)
	{
		if (fread(buffer, size, 1, file) != 1)
		{
			freef(buffer);
			fclose(file);
			return NULL;
		}

		buffer[size] = '\0'; /* zero terminate buffer */
	}

	fclose(file);
	return buffer;
}

size_t tb_file_read_buf(const char* path, const char* mode, char* buf, size_t max_len)
{
	FILE* file = fopen(path, mode);
	if (!file) return 0;

	size_t size = fread(buf, 1, max_len - 1, file);

	buf[size] = '\0'; /* zero terminate buffer */

	fclose(file);
	return size + 1;
}

size_t tb_file_copy(const char* src_path, const char* dst_path)
{
	char buffer[TB_FILE_COPY_BUFFER_SIZE];

	FILE* src = fopen(src_path, "rb");
	FILE* dst = fopen(dst_path, "wb");

	size_t read, wrote = 0;
	if (src && dst)
	{
		while ((read = fread(buffer, 1, TB_FILE_COPY_BUFFER_SIZE, src)))
			wrote += fwrite(buffer, 1, read, dst);
	}

	if (src) fclose(src);
	if (dst) fclose(dst);
	return wrote;
}


#endif /* !TB_FILE_IMPLEMENTATION */

/*
MIT License

Copyright (c) 2020 oliverjakobs

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/