#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ROWS 30
#define COLS 60
#define MAX  100

// ---------- Shape Types ----------
typedef enum { LINE, RECTANGLE, TRIANGLE, CIRCLE } ShapeType;

// ---------- Shape Structure ----------
typedef struct {
    ShapeType type;
    int x1, y1;
    int x2, y2;
    int w, h;
    int r;
    int active;
} Shape;

Shape shapes[MAX];
int shapeCount = 0;

char canvas[ROWS][COLS];

// ---------- Safe plot ----------
void plot(int row, int col) {
    if (row >= 0 && row < ROWS && col >= 0 && col < COLS)
        canvas[row][col] = '*';
}

// ---------- Canvas ----------
void initializeCanvas() {
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            canvas[i][j] = ' ';
}

void displayPicture() {
    printf("\n");
    // top border
    printf("+");
    for (int j = 0; j < COLS; j++) printf("-");
    printf("+\n");

    for (int i = 0; i < ROWS; i++) {
        printf("|");
        for (int j = 0; j < COLS; j++)
            printf("%c", canvas[i][j]);
        printf("|\n");
    }

    // bottom border
    printf("+");
    for (int j = 0; j < COLS; j++) printf("-");
    printf("+\n");
}

// ---------- Drawing Functions ----------

// Bresenham – works for ALL directions including diagonals
void drawLine(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx - dy, e2;
    for (;;) {
        plot(y1, x1);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 <  dx) { err += dx; y1 += sy; }
    }
}

void drawRectangle(int x, int y, int w, int h) {
    if (w <= 0 || h <= 0) { printf("Width and height must be > 0.\n"); return; }
    drawLine(x,     y,     x+w-1, y    );  // top
    drawLine(x,     y+h-1, x+w-1, y+h-1);  // bottom
    drawLine(x,     y,     x,     y+h-1);  // left
    drawLine(x+w-1, y,     x+w-1, y+h-1);  // right
}

// Triangle: apex at (x,y), height h, base at bottom – fully closed
void drawTriangle(int x, int y, int h) {
    if (h <= 0) { printf("Height must be > 0.\n"); return; }
    int bx1 = x - (h - 1);
    int bx2 = x + (h - 1);
    int by  = y + h - 1;
    drawLine(x,   y,  bx1, by);   // left side
    drawLine(x,   y,  bx2, by);   // right side
    drawLine(bx1, by, bx2, by);   // base  ← was missing
}

// Circle: uses floating-point angle scan for uniform density
void drawCircle(int cx, int cy, int r) {
    if (r <= 0) { printf("Radius must be > 0.\n"); return; }
    // aspect correction: terminal chars are ~2x taller than wide
    for (int angle = 0; angle < 360; angle++) {
        double rad = angle * M_PI / 180.0;
        int col = cx + (int)round(r * cos(rad));
        int row = cy + (int)round(r * 0.5 * sin(rad));
        plot(row, col);
    }
}

// ---------- Redraw ----------
void redraw() {
    initializeCanvas();
    for (int i = 0; i < shapeCount; i++) {
        if (!shapes[i].active) continue;
        Shape s = shapes[i];
        switch (s.type) {
            case LINE:      drawLine     (s.x1, s.y1, s.x2, s.y2); break;
            case RECTANGLE: drawRectangle(s.x1, s.y1, s.w,  s.h ); break;
            case TRIANGLE:  drawTriangle (s.x1, s.y1, s.h       ); break;
            case CIRCLE:    drawCircle   (s.x1, s.y1, s.r       ); break;
        }
    }
}

// ---------- List shapes ----------
void listShapes() {
    if (shapeCount == 0) { printf("No shapes yet.\n"); return; }
    printf("\n%-5s %-10s %s\n", "Index", "Type", "Parameters");
    printf("-------------------------------\n");
    for (int i = 0; i < shapeCount; i++) {
        if (!shapes[i].active) { printf("%-5d (deleted)\n", i); continue; }
        Shape s = shapes[i];
        printf("%-5d ", i);
        switch (s.type) {
            case LINE:      printf("%-10s x1=%d y1=%d x2=%d y2=%d\n",
                                   "Line", s.x1,s.y1,s.x2,s.y2); break;
            case RECTANGLE: printf("%-10s x=%d y=%d w=%d h=%d\n",
                                   "Rectangle",s.x1,s.y1,s.w,s.h); break;
            case TRIANGLE:  printf("%-10s x=%d y=%d h=%d\n",
                                   "Triangle",s.x1,s.y1,s.h); break;
            case CIRCLE:    printf("%-10s cx=%d cy=%d r=%d\n",
                                   "Circle",s.x1,s.y1,s.r); break;
        }
    }
}

// ---------- Read shape params ----------
Shape readShape() {
    Shape s = {0};
    int choice;
    printf("  1.Line  2.Rectangle  3.Triangle  4.Circle\n  Choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            s.type = LINE;
            printf("  Enter x1 y1 x2 y2: ");
            scanf("%d %d %d %d", &s.x1, &s.y1, &s.x2, &s.y2);
            break;
        case 2:
            s.type = RECTANGLE;
            printf("  Enter x y width height: ");
            scanf("%d %d %d %d", &s.x1, &s.y1, &s.w, &s.h);
            break;
        case 3:
            s.type = TRIANGLE;
            printf("  Enter apex-x apex-y height: ");
            scanf("%d %d %d", &s.x1, &s.y1, &s.h);
            break;
        case 4:
            s.type = CIRCLE;
            printf("  Enter cx cy radius: ");
            scanf("%d %d %d", &s.x1, &s.y1, &s.r);
            break;
        default:
            printf("  Invalid shape choice.\n");
            s.type = -1;
    }
    s.active = 1;
    return s;
}

// ---------- Add ----------
void addObject() {
    if (shapeCount >= MAX) { printf("Shape limit reached!\n"); return; }
    Shape s = readShape();
    if ((int)s.type == -1) return;
    shapes[shapeCount++] = s;
    redraw();
    printf("  Shape added at index %d.\n", shapeCount - 1);
}

// ---------- Delete ----------
void deleteObject() {
    listShapes();
    if (shapeCount == 0) return;
    int index;
    printf("Enter index to delete: ");
    scanf("%d", &index);
    if (index < 0 || index >= shapeCount || !shapes[index].active) {
        printf("Invalid index!\n"); return;
    }
    // shift shapes down
    for (int i = index; i < shapeCount - 1; i++)
        shapes[i] = shapes[i + 1];
    shapeCount--;
    redraw();
    printf("  Shape deleted.\n");
}

// ---------- Modify ----------
void modifyObject() {
    listShapes();
    if (shapeCount == 0) return;
    int index;
    printf("Enter index to modify: ");
    scanf("%d", &index);
    if (index < 0 || index >= shapeCount || !shapes[index].active) {
        printf("Invalid index!\n"); return;
    }
    printf("  Enter new shape details:\n");
    Shape s = readShape();
    if ((int)s.type == -1) return;
    shapes[index] = s;
    redraw();
    printf("  Shape modified.\n");
}

// ---------- MAIN ----------
int main() {
    initializeCanvas();
    int choice;

    while (1) {
        printf("\n--- MENU ---\n");
        printf("1. Add Shape\n");
        printf("2. Delete Shape\n");
        printf("3. Modify Shape\n");
        printf("4. Display Canvas\n");
        printf("5. List Shapes\n");
        printf("6. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addObject();    break;
            case 2: deleteObject(); break;
            case 3: modifyObject(); break;
            case 4: displayPicture(); break;
            case 5: listShapes();   break;
            case 6: printf("Bye!\n"); return 0;
            default: printf("Invalid choice.\n");
        }
    }
}