#define SHM_NAME "IPCD_SHM"
#define SEM_NAME "IPCD_SEM"

// structure for the shared memory segment

typedef struct {
	
	int input;
	int result;

} shmseg_t;
