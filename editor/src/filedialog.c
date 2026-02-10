/* ──────────────────────────────────────────────────────────────
 *  Native file dialog – platform implementations
 *
 *  This file is intentionally kept separate from raylib so that
 *  <windows.h> symbols (Rectangle, CloseWindow, ShowCursor …)
 *  never collide with raylib's identifiers.
 * ────────────────────────────────────────────────────────────── */
#include "filedialog.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PATH_LEN 512

/* ════════════════════════════════════════════════════════════════
 *  Windows: Win32 common-dialogs
 * ════════════════════════════════════════════════════════════════ */
#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>

bool nativeFileDialog(char *outPath, int outSize, int mode,
                      const char *title, const char *filter)
{
    /* Build a Win32-style double-null-terminated filter string.
     * Input: "Map files | *.txt"  or  "Images | *.png *.jpg *.bmp"
     * Win32 needs: "Map files\0*.txt\0\0"                         */
    char filterBuf[512];
    memset(filterBuf, 0, sizeof(filterBuf));
    strncpy(filterBuf, filter, sizeof(filterBuf) - 4);

    /* Replace " | " separator with \0 */
    char *bar = strstr(filterBuf, " | ");
    if (bar) {
        *bar = '\0';                         /* end the description */
        char *exts = bar + 3;                /* skip " | " */
        memmove(bar + 1, exts, strlen(exts) + 1);
        /* Replace spaces between extensions with semicolons */
        for (char *p = bar + 1; *p; p++)
            if (*p == ' ') *p = ';';
    }
    /* Ensure double-null termination at the very end */
    {
        size_t pos = 0;
        while (pos < sizeof(filterBuf) - 2) {
            if (filterBuf[pos] == '\0') {
                /* Skip past the embedded \0 to find second string */
                pos++;
                /* Find end of second string */
                while (pos < sizeof(filterBuf) - 1 && filterBuf[pos] != '\0')
                    pos++;
                break;
            }
            pos++;
        }
        /* pos now points at the \0 ending the second string.
         * Make sure the byte after it is also \0. */
        if (pos + 1 < sizeof(filterBuf))
            filterBuf[pos + 1] = '\0';
    }

    OPENFILENAMEA ofn;
    char fileBuf[MAX_PATH_LEN];
    memset(fileBuf, 0, sizeof(fileBuf));
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize  = sizeof(ofn);
    ofn.hwndOwner    = NULL;
    ofn.lpstrFile    = fileBuf;
    ofn.nMaxFile     = sizeof(fileBuf);
    ofn.lpstrFilter  = filterBuf;
    ofn.nFilterIndex = 1;
    ofn.lpstrTitle   = title;
    ofn.Flags        = OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

    BOOL ok;
    if (mode == 1) {
        ofn.Flags |= OFN_OVERWRITEPROMPT;
        ok = GetSaveFileNameA(&ofn);
    } else {
        ofn.Flags |= OFN_FILEMUSTEXIST;
        ok = GetOpenFileNameA(&ofn);
    }
    if (!ok) return false;

    strncpy(outPath, fileBuf, outSize - 1);
    outPath[outSize - 1] = '\0';
    return true;
}

/* ════════════════════════════════════════════════════════════════
 *  Linux / WSL: zenity
 * ════════════════════════════════════════════════════════════════ */
#else

bool nativeFileDialog(char *outPath, int outSize, int mode,
                      const char *title, const char *filter)
{
    char cmd[1024];
    if (mode == 1)
        snprintf(cmd, sizeof(cmd),
                 "zenity --file-selection --save --confirm-overwrite "
                 "--title=\"%s\" --file-filter=\"%s\" 2>/dev/null", title, filter);
    else
        snprintf(cmd, sizeof(cmd),
                 "zenity --file-selection "
                 "--title=\"%s\" --file-filter=\"%s\" 2>/dev/null", title, filter);

    FILE *fp = popen(cmd, "r");
    if (!fp) return false;

    char buf[MAX_PATH_LEN] = {0};
    if (!fgets(buf, sizeof(buf), fp)) { pclose(fp); return false; }
    int status = pclose(fp);
    if (status != 0) return false;

    /* Strip trailing newline */
    size_t len = strlen(buf);
    while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r'))
        buf[--len] = '\0';
    if (len == 0) return false;

    strncpy(outPath, buf, outSize - 1);
    outPath[outSize - 1] = '\0';
    return true;
}

#endif /* _WIN32 */
