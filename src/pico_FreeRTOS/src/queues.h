#ifndef QUEUES_H
#define QUEUES_H

#define POTHOLE 1
#define ROAD_DEPRESSION 2
#define SLIPPING 3
#define GPS 4

struct message
{
    double latitude;
    double longitude;
    double speed;
    int message_type;
};

struct gps
{
    double latitude;
    double longitude;
};

void initQueues();
void message_enqueue(struct message x);
int message_queue_dequeue(struct message* x);
void gps_queue_overwrite(struct gps x);
int gps_queue_peek(struct gps* x);

#endif
