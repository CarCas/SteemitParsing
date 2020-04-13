#define COMMENT "COMMENT"
#define VOTE "VOTE"
#define FOLLOW "FOLLOW"
#define REBLOG "REBLOG"
#define CARD "CARD"
#define SM_SELL "SM_SELL"
#define TRANSFER "TRANSFER"
#define TRANSFER_TO_VESTING "TRANSFER_TO_VESTING"
#define WITHDRAW_VESTING "WITHDRAW_VESTING"
#define LIMIT_ORDER_CREATE "LIMIT_ORDER_CREATE"
#define LIMIT_ORDER_CANCEL "LIMIT_ORDER_CANCEL"
#define PRICE "PRICE"
#define FEED_PUBLISH "FEED_PUBLISH"
#define CUSTOM_JSON "CUSTOM_JSON"

#define WHALE_LOWER 350000
#define SHARK_LOWER 100000
#define DOLPHIN_LOWER 5000
#define MINNOW_LOWER 500
#define PLANKTON_LOWER 0

#define VS_TO_SP 0.0005035760389197656

#define MEGA 1073741824

#define FOLDER_PRODUCED string("./caserio_informations_produced")

#define CMT_HUBS_4_AUTHOR_DIR   FOLDER_PRODUCED + "/comment_hubs_4_author"

#define CMT_HUBS_FILE           FOLDER_PRODUCED + "/comment_hubs.dat"


#define CMT_HUBS_DIR        FOLDER_PRODUCED + "/comment_hubs"

#define CMT_HUBS_WHALE      "/whale_hubs.dat"
#define CMT_HUBS_SHARK      "/shark_hubs.dat"
#define CMT_HUBS_DOLPHIN    "/dolphin_hubs.dat"
#define CMT_HUBS_MINNOW     "/minnow_hubs.dat"
#define CMT_HUBS_PLANKTON   "/plankton_hubs.dat"


#define OPERATIONS_FOLDER   FOLDER_PRODUCED + "/operations_log"

#define VOTES_FILE          OPERATIONS_FOLDER + "/votes.csv"
#define COMMENTS_FILE       OPERATIONS_FOLDER + "/comments.csv"
#define FOLLOWS_FILE        OPERATIONS_FOLDER + "/follows.csv"
#define REBLOGS_FILE        OPERATIONS_FOLDER + "/reblogs.csv"
#define COMMON_INFO_FILE    OPERATIONS_FOLDER + "/common_info_users.csv"


#define FIRST_N_USERS_FOLDER    FOLDER_PRODUCED + "/sorted_users"

#define SBD_USERS               "sbd_users.csv"
#define VEST_USERS              "vest_users.csv"
#define BALANCE_USERS           "balance_users.csv"

#define FREQUENCIES         FOLDER_PRODUCED + "/frequencies.dat"

#define N_MAX_USERS 10000

#define DELIMITER char(255)
