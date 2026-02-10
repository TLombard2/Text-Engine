#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <stdbool.h>

/* Open a native file-picker dialog.
 *   mode   : 0 = open file, 1 = save file
 *   title  : dialog window title
 *   filter : platform description + glob pattern, e.g.
 *            "Map files | *.txt"   or   "Images | *.png *.jpg *.bmp"
 *   outPath: receives the chosen path (must be at least outSize bytes)
 * Returns true if the user picked a file. */
bool nativeFileDialog(char *outPath, int outSize, int mode,
                      const char *title, const char *filter);

#endif /* FILEDIALOG_H */
