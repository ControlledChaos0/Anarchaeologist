extern ANISPRITE instructArrow;
extern ANISPRITE aButton;
extern ANISPRITE bButton;
extern ANISPRITE upDown;
extern ANISPRITE leftRight;
extern int instructScreenNum;

#define INSTRUCTARROWOAM 1
#define ABUTTONOAM 2
#define BBUTTONOAM 3
#define UPDOWNOAM 4
#define LEFTRIGHTOAM 5

void initInstruct();
void updateInstruct();

void initScreen1();
void initScreen2();

void updateScreen1();
void updateScreen2();

void initInstructArrow1();
void updateInstructArrow1();
void initInstructArrow2();
void updateInstructArrow2();

void initAButton();
void initBButton();
void initUpDown();
void initLeftRight();

void updateAButton();
void updateBButton();
void updateUpDown();
void updateLeftRight();