/*
 * catsem.c
 *
 * 30-1-2003 : GWA : Stub functions created for CS161 Asst1.
 *
 * NB: Please use SEMAPHORES to solve the cat syncronization problem in 
 * this file.
 */

//COMMENTS: P() is used in place of wait(), V() is used in place of signal().


/* 
 * Libraries
 */

#include <types.h>
#include <lib.h>
#include <test.h>
#include <thread.h>
#include <synch.h>

/*
 * Constants
 */

/*
 * Number of food bowls.
 */

#define NFOODBOWLS 2

/*
 * Number of cats.
 */

#define NCATS 6

/*
 * Number of mice.
 */

#define NMICE 2

/*
 * Number of times each animal is allowed to eat.
 */

#define TURNS 5

/*
 * List of animal names
 */
static const char *const cat_players[NCATS] = {"Tom", "Fatso", "Fluffy", "Meowth", "Furball", "Clawdette" };

static const char *const mouse_players[NMICE] = {"Jerry", "Stewart"};

/*
 * Instantiate Semaphores
 */

static struct semaphore *mutex;
static struct semaphore *dish_mutex;
static struct semaphore *no_cat_eat_mutex;
static struct semaphore *cats_waiting;
static struct semaphore *mice_waiting;
static struct semaphore *cats_done;
static struct semaphore *mice_done;
static struct semaphore *finished;

/*
 * Instantiate counter variables.
 */

static volatile int dish1_busy = 0;
static volatile int dish2_busy = 0;
static volatile int no_cat_eat = 1;
static volatile int all_dishes_avail = 1;
static volatile int no_mouse_eat = 1;

static volatile int cat_wait_count = 0;
static volatile int mice_wait_count = 0;
static volatile int cats_eating = 0;

/*
 * Function Definitions 
 */


/*
 * setup()
 *
 * Arguments:
 *      None
 *
 * Returns:
 *      Void
 *
 * Notes:
 *      Initializes all semaphores
 *
 */
void setup(){
        mutex = sem_create("mutex", 1);
        if(mutex==NULL){
                panic("Out Of Memory");
        }
        cats_waiting = sem_create("cats_waiting", 0);
        if(cats_waiting==NULL){
                panic("Out Of Memory");
        }
        mice_waiting = sem_create("mice waiting", 0);
        if(mice_waiting==NULL){
                panic("Out Of Memory");
        }
        dish_mutex = sem_create("dish mutex", 1);
        if(dish_mutex==NULL){
                panic("Out Of Memory");
        }
        cats_done = sem_create("cats done", 0);
        if(cats_done==NULL){
                panic("Out Of Memory");
        }
        mice_done = sem_create("mice done", 0);
        if(mice_done==NULL){
                panic("Out Of Memory");
        }
        no_cat_eat_mutex = sem_create("no cat eat mutex", 1);
        if(no_cat_eat_mutex==NULL){
                panic("Out Of Memory");
        }
        finished = sem_create("finished", 0);
        if(finished==NULL){
                panic("Out Of Memory");
        }
}

/*
 * cleanup()
 *
 * Arguments:
 *      None
 *
 * Returns:
 *      void
 *
 * Notes:
 *      Deletes all instantiated semaphores
 *
 */
void cleanup(){
        int i;
        for(i = 0; i<NCATS+NMICE; i++){
                P(finished); //waits for signal from each thread to indicate program is finished
        }
        sem_destroy(mutex);
	sem_destroy(dish_mutex);
     	sem_destroy(no_cat_eat_mutex);
	sem_destroy(cats_waiting);
        sem_destroy(mice_waiting);
        sem_destroy(cats_done);
        sem_destroy(mice_done);
        sem_destroy(finished);

    
        kprintf("\nTerminating Program.\n"); //Inform user program has finished
}

/*
 * FirstCatNoMouse()
 *
 * Arguments:
 *      const char *animal: holds the ID of the cat
 *      int *first_cat_eat
 *
 * Returns:
 *      Void
 *
 * Notes:
 *      This function allows the first cat to claim a spot in the kitchen
 *
 */
static void FirstCatNoMouse(const char *animal, int *first_cat_eat){

        P(mutex); //initial mutex to protext all_dishes_avail

        if(all_dishes_avail){ //if all dishes are available, this cat claims a dish
                all_dishes_avail = 0;
                V(cats_waiting);
        }

        cat_wait_count++;
        V(mutex); 
        
        P(cats_waiting); //holds cats here if all dishes are not available
        P(no_cat_eat_mutex); //mutex for no_cat_eat section to pace cats better

        if(no_cat_eat){ //if no cat is eating, then must be first cat
                no_cat_eat = 0;
                *first_cat_eat = 1;
        } else {
                *first_cat_eat = 0;
        }
        cats_eating++; //# of cats actively eating
        V(no_cat_eat_mutex);

}

/*
 * FirstCat()
 *
 * Arguments:
 *      const char *animal: holds the id of the cat 
 *      int *first_cat_eat
 *      int *another_cat_eat
 *
 * Returns:
 *      Void
 *
 * Notes:
 *      First cat decides if another cat can join it in the kitchen
 *
 */
static void FirstCat(const char *animal, int *first_cat_eat, int *another_cat_eat){

        if(*first_cat_eat){
                P(mutex); //lock critical section of cat_wait_count
                if(cat_wait_count>2){ //allows a second cat to join if there is one in queue
                        *another_cat_eat = 1;
                        V(cats_waiting);
                }
                V(mutex);
        }

}

/*
 * CatDishes()
 *
 * Arguments:
 *      int *myDish: specifies which dish this cat will take
 *      const char *animal: holds the id of the cat
 *      int num: allows iteration through names
 *
 * Returns:
 *      Void.
 *
 * Notes:
 *      This cat in the kitchen claims a dish to eat from
 *
 */
static void CatDishes(int *myDish, const char *animal, int num){

        P(dish_mutex); //protect critical section of dish

        if(!dish1_busy){ //Claims dish 1 if not taken, else takes dish 2
                dish1_busy = 1;
                *myDish = 1;
        } else {
                assert(!dish2_busy);
                dish2_busy = 1;
                *myDish = 2;
        }

        V(dish_mutex);

        kprintf("\nCat %s is eating from dish %d",  cat_players[num], *myDish); //Cat eats
        clocksleep(1);
        kprintf("\nCat %s is finished eating from dish %d", cat_players[num], *myDish);

}

/*
 * ReleaseDishCat()
 *
 * Arguments:
 *      int *myDish: specifies which dish this cat holds
 *
 * Returns:
 *      Void.
 *
 * Notes:
 *      Releases the dish from the cat holding it
 *
 */
static void releaseDishCat(int *myDish){

        P(mutex); //protects cat_wait_count
        P(dish_mutex); //protects if block

        if(*myDish==1){ //releases dish 1 if that is dish taken by this cat, else dish 2
                dish1_busy = 0;
        } else { 
                assert(*myDish==2);
                dish2_busy = 0;
        }

        V(dish_mutex);
        cats_eating--;
        cat_wait_count--;
        V(mutex);

}

/*
 * switchTurnsCat()
 *
 * Arguments:
 *      None
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Prioritizes mice if cats are done eating, then cats, then sets all dishes available
 *
 */
static void switchTurnsCat(){
        
        P(mutex); //protect critical section

        if(mice_wait_count>0){ //switches to mice turn if mice are waiting
                V(mice_waiting);
        } else {
                if(cat_wait_count>0){ //keeps turn to cats if cats are waiting
                        V(cats_waiting);
                } else {
                        all_dishes_avail = 1; //else free for all
                }
        }

        V(mutex);
}

/*
 * catsLeaving()
 *
 * Arguments:
 *      const char *animal: holds the id of the cat
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Process for cats leaving the kitchen
 *
 */
static void catsLeaving(const char *animal, int *first_cat_eat, int *another_cat_eat){

        if(*first_cat_eat){

                if(*another_cat_eat){
                        //kprintf("\nwaiting on other cat")
                        P(cats_done); //if both cats are in the kitchen first cat must wait on other cat before first cat can leave
                }

                no_cat_eat = 1;
                switchTurnsCat();

        } else {

                V(cats_done); //other cat is ready to leave and signals to first cat

        }

}

/*
 * FirstMouseNoCat()
 *
 * Arguments:
 *      const char *animal: holds the id of the mouse
 *	int *first_mouse_eat
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function allows the first mouse to enter the kitchen
 *
 */
static void FirstMouseNoCat(const char *animal, int *first_mouse_eat){

        P(mutex); //protects critical section
        
        if(all_dishes_avail){ //if all dishes are available, this mouse claims one
                all_dishes_avail = 0;
                V(mice_waiting);
        }
        
        mice_wait_count++;
        V(mutex);
        P(mice_waiting); //holds mice here if they did not claim all_dishes_avail
 
        if(no_mouse_eat){ //if no mouse is eating, then must be first mouse.
                no_mouse_eat = 0;
                *first_mouse_eat = 1;
        } else {
                *first_mouse_eat = 0;
        }

}

/*
 * FirstMouse()
 *
 * Arguments:
 *      const char *animal: holds the id of the mouse 
 *	int *first_mouse_eat
 *	int *another_mouse_eat
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      First mouse decides if another mouse can join it in the kitchen
 *
 */
static void FirstMouse(const char *animal, int *first_mouse_eat, int *another_mouse_eat){
       
        if(*first_mouse_eat){
                P(mutex); //locks this critical section
                if(mice_wait_count>1){ //allows another mouse to join if there is one in the queue
                        *another_mouse_eat = 1;
                        V(mice_waiting);
                }
                V(mutex);
        }

}

/*
 * MouseDishes()
 *
 * Arguments:
 *      int *myDish: specifies which dish this cat will take
 *      const char *animal: holds the id of the cat
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This cat in the kitchen claims a dish to eat from
 *
 */
static void MouseDishes(int *myDish, const char *animal, int num){

        P(dish_mutex); //protects critical section of dish
        if(!dish1_busy){ //claims dish 1 if not busy else dish 2
                dish1_busy = 1;
                *myDish = 1;
        } else {
                assert(!dish2_busy);
                dish2_busy = 1;
                *myDish = 2;
        }

        V(dish_mutex);

        kprintf("\nMouse %s is eating from dish %d", mouse_players[num], *myDish); //mouse eats
        clocksleep(1);
        kprintf("\nMouse %s is finished eating from dish %d", mouse_players[num], *myDish);
}

/*
 * ReleaseDishMouse()
 *
 * Arguments:
 *      int *myDish: specifies which dish this mouse holds
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Releases the dish from the mouse holding it
 *
 */
static void releaseDishMouse(int *myDish){

        P(mutex); //protects mice_wait_count
        P(dish_mutex); //protects if block

        if(*myDish==1){ //releases dish 1 if taken by mouse, else dish 2
                dish1_busy = 0;
        } else { 
                assert(*myDish==2);
                dish2_busy = 0;
        }

        V(dish_mutex);
        mice_wait_count--;
        V(mutex);
}

/*
 * switchTurnsMouse()
 *
 * Arguments:
 *      None
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Prioritizes cats if mice are done eating, then mice, then sets all dishes available
 *
 */
static void switchTurnsMouse(){

        P(mutex); //protect critical section

        if(cat_wait_count>0){ //switches turn to cats if cats are waiting
                V(cats_waiting);
        } else {
                if(mice_wait_count>0){ //keeps turn to mice if mice are waiting
                        V(mice_waiting);
                } else {
                        all_dishes_avail = 1; //else free for all
                }
        }
        V(mutex);
}

/*
 * mouseLeaving()
 *
 * Arguments:
 *      const char *animal: holds the id of the mouse
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Process for mouse leaving the kitchen
 *
 */
static void mouseLeaving(const char *animal, int *first_mouse_eat, int *another_mouse_eat){

        if(*first_mouse_eat){

                if(*another_mouse_eat){ 
                        P(mice_done); //if another mouse is eating, first mouse waits until it is done
                }

                no_mouse_eat = 1;
                switchTurnsMouse();

        } else {

                V(mice_done); //if another mouse is eating, signals to first mouse that it is done

        }


}

/*
 * catsem()
 *
 * Arguments:
 *      void * unusedpointer: currently unused.
 *      unsigned long catnumber: holds the cat identifier from 0 to NCATS - 1.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Write and comment this function using semaphores.
 *
 */

static
void
catsem(void * unusedpointer, 
       unsigned long catnumber)
{
        /*
         * Avoid unused variable warnings.
         */

        (void) unusedpointer;
        (void) catnumber;

        
        int i;

        for(i = 0; i<TURNS; i++){
                int myDish = 0; 
                int first_cat_eat = 0;
                int another_cat_eat = 0;
                clocksleep(2); //cat plays
                FirstCatNoMouse(catnumber, &first_cat_eat);
                FirstCat(catnumber, &first_cat_eat, &another_cat_eat);
                CatDishes(&myDish, &cat_players, catnumber);
                releaseDishCat(&myDish);
                catsLeaving(catnumber, &first_cat_eat, &another_cat_eat);
        }
        V(finished); //signals that this cat is done


}
        

/*
 * mousesem()
 *
 * Arguments:
 *      void * unusedpointer: currently unused.
 *      unsigned long mousenumber: holds the mouse identifier from 0 to 
 *              NMICE - 1.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Write and comment this function using semaphores.
 *
 */

static
void
mousesem(void * unusedpointer, 
         unsigned long mousenumber)
{
        /*
         * Avoid unused variable warnings.
         */

        (void) unusedpointer;
        (void) mousenumber;


        int i;

        for(i = 0; i<TURNS; i++){
                int first_mouse_eat = 0;
                int another_mouse_eat = 0;
                int myDish = 0;
                clocksleep(2); //mouse plays
                FirstMouseNoCat(mousenumber, &first_mouse_eat);
                FirstMouse(mousenumber, &first_mouse_eat, &another_mouse_eat);
                MouseDishes(&myDish, &mouse_players, mousenumber);
                releaseDishMouse(&myDish);
                mouseLeaving(mousenumber, &first_mouse_eat, &another_mouse_eat);
        }
        V(finished); //signals that this mouse is done




}

/*
 * catmousesem()
 *
 * Arguments:
 *      int nargs: unused.
 *      char ** args: unused.
 *
 * Returns:
 *      0 on success.
 *
 * Notes:
 *      Driver code to start up catsem() and mousesem() threads.
 */

int
catmousesem(int nargs,
            char ** args)
{
        int index, error;
   
        /*
         * Avoid unused variable warnings.
         */

        (void) nargs;
        (void) args;
        setup();
        /*
         * Start NCATS catsem() threads.
         */

        for (index = 0; index < NCATS; index++) {
           
                error = thread_fork("catsem Thread", 
                                    NULL, 
                                    index, 
                                    catsem, 
                                    NULL
                                    );
                
                /*
                 * panic() on error.
                 */

                if (error) {
                 
                        panic("catsem: thread_fork failed: %s\n", 
                              strerror(error)
                              );
                }
        }
        
        /*
         * Start NMICE mousesem() threads.
         */

        for (index = 0; index < NMICE; index++) {
   
                error = thread_fork("mousesem Thread", 
                                    NULL, 
                                    index, 
                                    mousesem, 
                                    NULL
                                    );
                
                /*
                 * panic() on error.
                 */

                if (error) {
         
                        panic("mousesem: thread_fork failed: %s\n", 
                              strerror(error)
                              );
                }
        }

        cleanup();
        
        return 0;
}


/*
 * End of catsem.c
 */
