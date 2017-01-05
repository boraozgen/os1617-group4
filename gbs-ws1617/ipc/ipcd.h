#define SHM_NAME "IPCD_SHM"
#define SEM_NAME_BEGIN "IPCD_SEM_BEGIN"
#define SEM_NAME_END "IPCD_SEM_END"

// structure for the shared memory segment

typedef struct {
	
	int input;
	int result;

} shmseg_t;
