#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>

#include "arg.h"
#include "xdiff/xdiff.h"

// Documentation can be found here
// http://www.chemie.fu-berlin.de/chemnet/use/info/diff/diff_3.html#SEC13

char *argv0;
int seen_hunk;

void
die(const char *errstr, ...)
{
        va_list ap;

        va_start(ap, errstr);
        vfprintf(stderr, errstr, ap);
        va_end(ap);
        exit(1);
}

void *
xmalloc(size_t len)
{
	void *p = malloc(len);

	if (!p)
		die("Out of memory\n");

	return p;
}

void print_ed_cmd (char cmd, long s1, long e1, long s2, long e2)
{
        // XXX Coalesce all the writes to stdio ?
        if (s1 == e1) printf("%ld", s1); else printf("%ld,%ld", s1, e1);
        putchar(cmd);
        if (s2 == e2) printf("%ld", s2); else printf("%ld,%ld", s2, e2);
        putchar('\n');
}

int print_hunk (long start_a, long count_a, long start_b, long count_b, void *cb_data)
{
        int s1, e1, s2, e2;

        // Adjust the start-end ranges
        s1 = count_a? start_a + 1: start_a;
        e1 = start_a + count_a;

        s2 = count_b? start_b + 1: start_b;
        e2 = start_b + count_b;

        if (count_b == 0)
                // Delete lines from a
                print_ed_cmd('d', s1, e1, e2, e2);
        else if (count_a == 0)
                // Add lines to a
                print_ed_cmd('a', e1, e1, s2, e2);
        else
                // Change lines
                print_ed_cmd('c', s1, e1, s2, e2);

        seen_hunk = 1;

        return 1;
}

void read_mmfile (mmfile_t *mf, const char *path)
{
        FILE *fp;
        struct stat st;
        size_t size;

        if (stat(path, &st))
                die("Could not stat %s\n", path);

        size = st.st_size;

        fp = fopen(path, "r");
        if (fp == NULL)
                die("Could not open %s\n", path);

        mf->ptr = xmalloc(size);
        mf->size = size;

        fread(mf->ptr, 1, size, fp);
        fclose(fp);
}

void usage ()
{
        die("Usage: %s [OPTION]... file1 file2\n"
            "\n"
            "\t-1\tuse the patience diff algorithm\n"
            "\t-2\tuse the histogram diff algorithm\n"
            "\t-Z\tignore white space at line end\n"
            "\t-b\tignore changes in the amount of white space\n"
            "\t-w\tignore all white space\n"
            "\t-d\ttry hard to find a smaller set of changes\n"
            "\t-B\tignore changes where lines are all blank\n",
            argv0);
}

int main (int argc, char **argv)
{
        xpparam_t param = { 0 };
        xdemitconf_t econf = { 0, 0, 0, NULL, NULL, print_hunk };
        xdemitcb_t ecb = { NULL, NULL, };

        param.flags = XDF_COMPACTION_HEURISTIC | XDF_INDENT_HEURISTIC;

        ARGBEGIN {
        case '1':
        case '2':
                param.flags &= ~XDF_DIFF_ALGORITHM_MASK;
                param.flags |= (ARGC() == '1') ? XDF_PATIENCE_DIFF : XDF_HISTOGRAM_DIFF;
                break;
        case 'Z':
                param.flags |= XDF_IGNORE_WHITESPACE_AT_EOL;
                break;
        case 'b':
                param.flags |= XDF_IGNORE_WHITESPACE_CHANGE;
                break;
        case 'w':
                param.flags |= XDF_IGNORE_WHITESPACE;
                break;
        case 'd':
                param.flags |= XDF_NEED_MINIMAL;
                break;
        case 'B':
                param.flags |= XDF_IGNORE_BLANK_LINES;
                break;
        default:
                usage();
        } ARGEND;

        if (argc != 2)
                usage();

        mmfile_t f1, f2;
        read_mmfile(&f1, argv[0]);
        read_mmfile(&f2, argv[1]);

        seen_hunk = 0;
        xdl_diff(&f1, &f2, &param, &econf, &ecb);

        return (seen_hunk ? EXIT_FAILURE : EXIT_SUCCESS);
}
