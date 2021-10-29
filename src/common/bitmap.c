#include <common/bitmap.h>
#include <common/string.h>

void init_bitmap(BitmapCell *bitmap, usize size) {
    memset(bitmap, 0, BITMAP_TO_NUM_CELLS(size) * sizeof(BitmapCell));
}
