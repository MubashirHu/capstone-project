
#define POTHOLE 1
#define ROAD_DEPRESSION 2
#define SLIPPING 3
#define GPS 4

struct message
{
    char utc_time[11];
    char latitude[12];
    char latitude_direction;
    char longitude[13];
    char longitude_direction;
    double speed;
    int message_type;
};

struct gps
{
    char utc_time[11];
    char latitude[12];
    char latitude_direction;
    char longitude[13];
    char longitude_direction;
    int positioning_status;
};

void initQueues();
void message_enqueue(struct message x);
int message_queue_dequeue(struct message* x);
void gps_queue_overwrite(struct gps x);
int gps_queue_peek(struct gps* x);
