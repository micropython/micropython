#include<stdio.h>
#include<stdlib.h>
#include<string.h>

static int sep_path_array(const char *path, char **arpath) {
    int cnt = 0;
    char *str = NULL;
    char *p1, *p2;

    if (path[strlen(path) - 1] != '/') {
        str = malloc(strlen(path) + 1 + 1);
        memcpy(str, path, strlen(path));
        str[strlen(path)] = '/';
        str[strlen(path) + 1] = '\0';
        p2 = str;
    } else {
        p2 = (char *)path;
    }

    while (1) {
        p1 = strchr(p2, '/');
        if (p1) {
            if (cnt) {
                arpath[cnt] = malloc(p1 - p2 + 1);
                memset(arpath[cnt], 0, p1 - p2 + 1);
                memcpy(arpath[cnt], p2, p1 - p2);
                cnt++;
            }
            p1++;
        } else {
            break;
        }

        p2 = strchr(p1, '/');
        if (p2) {
            arpath[cnt] = malloc(p2 - p1 + 1);
            memset(arpath[cnt], 0, p2 - p1 + 1);
            memcpy(arpath[cnt], p1, p2 - p1);
            cnt++;
            p2++;
        } else {
            break;
        }
    }

    free(str);
    return cnt;
}


static char *calc_relative_path(const char *patha, const char *pathb) {
    char *ret = NULL;
    char *arpatha[256];
    int patha_num = sep_path_array(patha, arpatha);
    char *arpathb[256];
    int pathb_num = sep_path_array(pathb, arpathb);

    int i, j, k = 0;

    j = (patha_num > pathb_num) ? pathb_num : patha_num;
    for (i = 0; i < j; i++) {
        if (!k && !strcmp(arpatha[i], arpathb[i])) {
            k = 1;
        } else if (k && strcmp(arpatha[i], arpathb[i])) {
            k = 1;
            i--;
            break;
        }
    }

    ret = malloc(256);
    memset(ret, 0, 256);
    if (k) {
        if (i == j)
            i--;
        strcat(ret, "./");
        for (k = 0; k < (patha_num - (i + 1)); k++) {
            strcat(ret, "../");
        }
        for (k = i + 1; k < pathb_num; k++) {
            strcat(ret, arpathb[k]);
            strcat(ret, "/");
        }
    } else {
        for (k = 0; k < patha_num; k++) {
            strcat(ret, "../");
        }
        for (k = 0; k < pathb_num; k++) {
            strcat(ret, arpathb[k]);
            strcat(ret, "/");
        }
    }

    ret[strlen(ret) - 1] = 0;

    for (i = 0; i < patha_num; i++) {
        free(arpatha[i]);
    }
    for (j = 0; j < pathb_num; j++) {
        free(arpathb[j]);
    }

    return ret;
}

int main(int argc, char **argv) {
    if (argc != 3)
        return -1;

    char *r = calc_relative_path(argv[1], argv[2]);

    printf("%s\n", r);

    free(r);

    return 0;
}

