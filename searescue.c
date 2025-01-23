/*
 * Implement the missing functionality in this file. DO NOT CHANGE ANY OTHER FILE!
 * Add your code only inside the given TODO borders
 */

#include "searescue.h"
#include <stdlib.h>

WINDOW *game_window;
char game_map[MAP_LENGTH][MAP_WIDTH];

pthread_t player_tid;
pthread_t *shark_tids;
pthread_t *debri_tids;
pthread_t *diver_tids;
pthread_t oxygen_tid;


void *rvalue_player = 0;
void *rvalue_oxygen = 0;
void **rvalue_enemies;
void **rvalue_divers;

time_t time1;

position player_position = {MAP_WIDTH / 2, MAP_LENGTH / 2};

int lifepoints = 100;
int points = 0;
int oxygen = 100;
int divers_collected = 0;
int remaining_divers = 0;
int divers_collected_flag = TRUE;

int number_sharks = 7;
int number_divers = 3;
int number_debris = 5;

int OXYGEN_DEPLETION_RATE = 1;
int OXYGEN_REPLENISHMENT_RATE = 5;
int OXYGEN_MAX_CAPACITY = 100;

void *playerLogic() {
    game_map[player_position.y_][player_position.x_] = (char) PLAYER;

    char direction = 'l';
    char prev_direction = 'l';

    int c;
    keypad(stdscr, TRUE);
    noecho();
    timeout(1);
    while (true) {
        c = getch();
        switch (c) {
            case 's':
                direction = 'd';
                break;
            case 'w':
                direction = 'u';
                break;
            case 'a':
                direction = 'l';
                break;
            case 'd':
                direction = 'r';
                break;
            default:
                break;
        }
        if (c == 'q' || oxygen <= 0) {
            lifepoints = 0;
            continue;
        }

        usleep(100000);

        // TODO (8):
        // - If the player has collected ALL divers and is near the surface,
        // - the divers are picked up by a rescue boat. (Hint: isNearSurface(), player_position, diversCollected())
        // - Once the divers are rescued by the boat the score should increase for every diver and
        // - their corresponding threads should somehow terminate
        // - also set divers_collected_flag to 1
        if (diversCollected() && isNearSurface(player_position)) {
                for (int i = 0; i < number_divers; i++) {
                  points += POINTS_DIVER;
                  ++divers_collected;
                }
                divers_collected_flag = TRUE;
            }
        
        // TODO END

        movePlayer(&direction, &prev_direction, &player_position, PLAYER);
    }
}

void *oxygenLogic() {
    while (lifepoints > 0) {
        usleep(100000);

        // TODO (2):
        // Implement the oxygen replenishment logic
        // If the player is near the surface the current amount of oxygen should increase by the specified replenishment rate.
        // else the current oxygen level should decrease by the depletion rate.  (Hint: isNearSurface)
        // Make sure that the oxygen level doesnt exceed the maximum capacity or decreases below zero.
        if (isNearSurface(player_position)) {
            // Replenish oxygen if near the surface
            oxygen += OXYGEN_REPLENISHMENT_RATE;
        } else {
            // Deplete oxygen if not near the surface
            oxygen -= OXYGEN_DEPLETION_RATE;
        }

        // Ensure oxygen level stays within bounds
        if (oxygen > OXYGEN_MAX_CAPACITY) {
            oxygen = OXYGEN_MAX_CAPACITY;
        } else if (oxygen < 0) {
            oxygen = 0;
        }

        // Ensure that oxygen stays within its bounds
        oxygen = (oxygen > OXYGEN_MAX_CAPACITY) ? OXYGEN_MAX_CAPACITY : oxygen;
        oxygen = (oxygen < 0) ? 0 : oxygen;
        // TODO END
    }

    return (void *) 777;
}

void *enemyDebri(parameters *params) {
    if (params->type_ != DEBRI)
        return (void *) -1;

    // TODO (4):
    // Spawn the debri correctly in the map. The given position gives the position of the top left field.
    // Make sure to initialize the other fields correctly!
    // spawnEnemy *only* initializes *one* field on the map.
    parameters paramsc = *params;
    free(params);

    while (lifepoints > 0) {
        usleep(300000);
        spawnEnemy(DEBRI, paramsc.pos_x_, paramsc.pos_y_);
        spawnEnemy(DEBRI, (paramsc.pos_x_+1)%MAP_WIDTH, paramsc.pos_y_);
        spawnEnemy(DEBRI, paramsc.pos_x_, (paramsc.pos_y_+1)%MAP_LENGTH);
        spawnEnemy(DEBRI, (paramsc.pos_x_+1)%MAP_WIDTH, (paramsc.pos_y_+1)%MAP_LENGTH);
    }
    // TODO END

    return (void *) 999;
}

void *enemyShark(parameters *params) {
    if (params->type_ != SHARK)
        return (void *) -1;

    // TODO (5):
    // -) set the shark position correctly
    // -) spawn the enemy shark correctly in the map

    position enemy_pos = {params->pos_x_, params->pos_y_};
    spawnEnemy(SHARK, enemy_pos.x_, enemy_pos.y_);
    free(params);

    // TODO END

    int nr_direction = 0;
    char prev_direction = 'l';

    while (lifepoints > 0) {
        usleep(300000);

        if (rand() % 5 == 0) {
            nr_direction = rand() % 4;
        }
        char direction;
        switch (nr_direction) {
            case 0:
                direction = 'l';
                movePlayer(&direction, &prev_direction, &enemy_pos, (char) SHARK);
                break;
            case 1:
                direction = 'r';
                movePlayer(&direction, &prev_direction, &enemy_pos, (char) SHARK);
                break;
            case 2:
                direction = 'u';
                movePlayer(&direction, &prev_direction, &enemy_pos, (char) SHARK);
                break;
            case 3:
                direction = 'd';
                movePlayer(&direction, &prev_direction, &enemy_pos, (char) SHARK);
                break;
            default:
                break;
        }
    }
    return (void *) 999;
}

void init_enemies(unsigned char type, int number_of_enemy_type) {
    pthread_attr_t enemy_attr;
    pthread_attr_init(&enemy_attr);

    // TODO (3): Implement the whole function:
    // - spawn all enemies of the given type (one thread per entity), make sure each one appears one it's random coordinates and has the type specified in type_
    // - use the attribute for thread creation
    // - use the shark_tids and debri_tid arrays to store the thread IDs in them
    // - all enemies have to move on their own and concurrently with the others!
    // - look at the provided data structures in the header. Use them!
    // - make sure to get a new random position for each enemy entity!
    // - Arguments needed? generate them via getRandPosX() and getRandPosY()
    // - pay attention to not call the getRandPos-functions unnecessary or more often then you need, since this will fail on the testsystem
    // - Furthermore, make sure to keep the lifetime of variables in mind and to not leak any memory!

    // It is ok for sharks and debris to overlap! It is also ok to have race conditions in your code. The testsystem will not test for this
    // and you will learn how to fix them in A2!

    for (int i = 0; i < number_of_enemy_type; i++) {
        position enemy_position;
        enemy_position.x_ = getRandPosX();
        enemy_position.y_ = getRandPosY();
        parameters *params = malloc(sizeof(parameters));
        params->pos_x_ = enemy_position.x_;
        params->pos_y_ = enemy_position.y_;
        params->type_ = type;

        if (type == SHARK) {
            pthread_create(&shark_tids[i], &enemy_attr, (void *(*)(void *))enemyShark, params);
        } else if (type == DEBRI) {
            pthread_create(&debri_tids[i], &enemy_attr, (void *(*)(void *))enemyDebri, params);
        }
    }

    pthread_attr_destroy(&enemy_attr);
    // TODO END

}

void *diver(parameters *params) {
    if (params->type_ != DIVER)
        return (void *) -1;

    // TODO (7):
    // - In general there should be NUMBER_DIVERS divers on the map at a time
    // - place the diver on the position taken from the function parameters
    // - the diver is only drawn on the map as long as it was not collected by the
    // - player. The diver threads should terminate IMMEDIATELY once the player
    // - reaches the surface with all divers on board. Think about what mechanism/characteristic which could be used for this.

    unsigned char position_x = params->pos_x_;
    unsigned char position_y = params->pos_y_;
    free(params);

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    // // TODO END
    int diver_collected = FALSE;

    while (1) {

        if (player_position.x_ == position_x && player_position.y_ == position_y && !diver_collected) {
            diver_collected = true;
            remaining_divers = remaining_divers - 1;
            clearPosition(position_x, position_y);
        }

        if (!diver_collected) {
            spawnDiver(position_x, position_y);
        }
    }
}

int start_game(int argc, char *argv[]) {
    init_screen();
    srand((unsigned int) time(&time1));
    diver_tids = malloc(sizeof(pthread_t) * number_divers);
    shark_tids = malloc(sizeof(pthread_t) * number_sharks);
    debri_tids = malloc(sizeof(pthread_t) * number_debris);

    rvalue_enemies = malloc(sizeof(void *) * (number_debris + number_sharks));
    rvalue_divers = malloc(sizeof(void *) * (number_divers));
    memset(rvalue_enemies, 0, sizeof(void *) * (number_debris + number_sharks));
    memset(rvalue_divers, 0, sizeof(void *) * number_divers);

    init_map();


    // TODO (1):
    // - spawn the player, oxygen refueling and enemies by creating separate threads for each entity
    // - Arguments needed? Attributes needed? Look at the playerLogic, oxygenLogic and the enemy functions
    // - look at the pthread_t variables from above... USE them!!
    // - Init all sharks with one call to init_enemies and all debris with another call to init_enemies
    // - Generate number_debris debris and number_sharks shark enemies
    // - Make sure to generate all sharks first, then all debris
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&player_tid, &attr, playerLogic, NULL);
    pthread_create(&oxygen_tid, &attr, oxygenLogic, NULL);
    pthread_attr_destroy(&attr);

    init_enemies(SHARK, number_sharks); // Initialize shark enemies
    init_enemies(DEBRI, number_debris); // Initialize debris enemies


    // TODO END

    refreshMap();

    // TODO (6):
    // - spawn the initial divers by creating diver threads (for the divers position call getRandPosX() and getRandPosY())
    // - if all the divers are collected and returned to the surface (there's a flag for that),
    // - new divers shall be spawned at new random positions
    // - pay attention to not call the getRandPos-functions unnecessary or more often then you need, since this will fail on the testsystem
    // - make sure that all diver threads are terminated and store the return value in rvalue_divers - Do this check for termination in this TODO
    // - have a look at the other TODOs from above - there might be interconnects
    // - Arguments needed? generate them via getRandPosX() and getRandPosY()
    // Spawn initial divers
    int i = 0;
    while (i < number_divers) {
        position diver_position;
        diver_position.x_ = getRandPosX();
        diver_position.y_ = getRandPosY();
        parameters *params = malloc(sizeof(parameters));
        params->pos_x_ = diver_position.x_;
        params->pos_y_ = diver_position.y_;
        params->type_ = DIVER;

        remaining_divers++;
        pthread_create(&diver_tids[i], NULL, (void *(*)(void *))diver, params);
        i++;
    }
    // TODO END
    divers_collected_flag = FALSE;

    while (lifepoints > 0) {
        usleep(10000);

        if (divers_collected_flag) {
            // TODO (9): Further implementations from TODO above
            for (int i = 0; i < number_divers; i++) {
                pthread_cancel(diver_tids[i]); // Terminate the diver thread
                pthread_join(diver_tids[i], &rvalue_divers[i]);
            }
            

            i = 0;
            memset(rvalue_divers, 0, sizeof(void *) * number_divers);
            while (i < number_divers) {
                position diver_position;
                diver_position.x_ = getRandPosX();
                diver_position.y_ = getRandPosY();
                parameters *params = malloc(sizeof(parameters));
                params->pos_x_ = diver_position.x_;
                params->pos_y_ = diver_position.y_;
                params->type_ = DIVER;

                remaining_divers++;
                pthread_create(&diver_tids[i], NULL, (void *(*)(void *))diver, params);
                i++;
            }
            // TODO END

            divers_collected_flag = FALSE;
        }

        refreshMap();
    }

    // TODO (10):
    // - make sure that all the running threads are terminated before returning from main and fetch their return values
    // - All threads which are able to return on their own should also do that so and therefore give back the corresponding return value
    // - Force the termination of threads which do not terminate on their own.
    // - have a closer look on the other TODOs. There might be some connections
    // - we want to make sure, that all threads are terminated for sure. But are all threads even joinable?

    pthread_cancel(player_tid);
    pthread_join(player_tid, &rvalue_player);
    //pthread_cancel(oxygen_tid);
    pthread_join(oxygen_tid, &rvalue_oxygen);

    for (int i = 0; i < number_sharks; i++) {
        //pthread_cancel(shark_tids[i]);
        pthread_join(shark_tids[i], &rvalue_enemies[i]);
    }

    for (int i = 0; i < number_debris; i++) {
        //pthread_cancel(debri_tids[i]);
        pthread_join(debri_tids[i], &rvalue_enemies[i + number_sharks]);
    }

    for (int i = 0; i < number_divers; i++) {
        pthread_cancel(diver_tids[i]);
        pthread_join(diver_tids[i], &rvalue_divers[i]);
    }

    // TODO END
    free(shark_tids);
    free(diver_tids);
    free(debri_tids);

    return end_game((void **) rvalue_player, (void *) rvalue_enemies, (void *) rvalue_divers, (void **) rvalue_oxygen);
}