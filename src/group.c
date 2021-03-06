#include "group.h"

struct link_Node *grp_createGroup(char *name, struct usr_UserData *user){
	if(name[0] != '&'){
		return NULL;
	}

	struct grp_Group *group;
	group = calloc(1, sizeof(struct grp_Group));
	if(group == NULL){
		log_logError("Error allocating group", WARNING);
		return NULL;
	}
	group->name = calloc(1, fig_Configuration.groupNameLength);
	if(group->name == NULL){
		log_logError("Error allocating group", WARNING);
		free(group);
		return NULL;
	}

    // Initalize mutex to prevent locking issues
    int ret = pthread_mutex_init(&group->groupMutex, NULL);
    if (ret < 0){
		free(group->name);
		free(group);
        log_logError("Error initalizing pthread_mutex.", ERROR);
        return NULL;
    }

	strncpy(group->name, name, fig_Configuration.groupNameLength-1);

	group->max = 100;
	group->users = calloc(group->max, sizeof(struct grp_GroupUser));
	if(group->users == NULL){
        log_logError("Error creating group", ERROR);
		free(group->name);
		free(group);
        return NULL;
	}

	// Add to main list
    pthread_mutex_lock(&serverLists.groupsMutex);
    struct link_Node *node = link_add(&serverLists.groups, group);
    pthread_mutex_unlock(&serverLists.groupsMutex);

	if(node == NULL){
		free(group->name);
		free(group->users);
		free(group);
        return NULL;
	}

	// Generate a default Channel
	chan_createChannel("#default", node);

	// Make selected user OPER of the group
	grp_addUser(node, user, 1);

	return node;
}

struct link_Node *grp_getGroup(char *name){
	struct link_Node *node;
	struct grp_Group *group;

	if(name[0] != '&')
		return NULL;

	pthread_mutex_lock(&serverLists.groupsMutex);
	
	for(node = serverLists.groups.head; node != NULL; node = node->next){
		group = node->data;
		pthread_mutex_lock(&group->groupMutex);

		if(!strncmp(group->name, name, fig_Configuration.groupNameLength)){
			pthread_mutex_unlock(&serverLists.groupsMutex);
			pthread_mutex_unlock(&group->groupMutex);
			return node;
		}

		pthread_mutex_unlock(&group->groupMutex);
	}

	pthread_mutex_unlock(&serverLists.groupsMutex);

	return NULL;
}

struct grp_GroupUser *grp_addUser(struct link_Node *groupNode, struct usr_UserData *user, int permLevel){
	struct grp_Group *group = groupNode->data;
	struct grp_GroupUser *grpUser = NULL;

	if(group == NULL)
		return NULL;

	// Already in group
	grpUser = grp_isInGroup(groupNode, user);
	if(grpUser != NULL)
		return grpUser;

	pthread_mutex_lock(&group->groupMutex);
	for(int i = 0; i < group->max; i++){
		if(group->users[i].user == NULL){
			group->users[i].user = user;
			group->users[i].permLevel = permLevel;
			grpUser = &group->users[i];
			break;
		}
	}
	pthread_mutex_unlock(&group->groupMutex);

	return grpUser;
}

struct grp_GroupUser *grp_isInGroup(struct link_Node *groupNode, struct usr_UserData *user){
	struct grp_Group *group = groupNode->data;
	struct grp_GroupUser *grpUser = NULL;

	if(group == NULL)
		return NULL;

	// Go thru each user and check
	pthread_mutex_lock(&group->groupMutex);
	for(int i = 0; i < group->max; i++){
		if(group->users[i].user == user){
			grpUser = &group->users[i];
			break;
		}
	}
	pthread_mutex_unlock(&group->groupMutex);
	
	return grpUser;
}

struct link_Node *grp_addChannel(struct link_Node *groupNode, struct chan_Channel *chan){
	if(groupNode == NULL || groupNode->data == NULL || chan == NULL)
		return NULL;
		
	struct grp_Group *group = groupNode->data;

	pthread_mutex_lock(&group->groupMutex);
	struct link_Node *chanNode = link_add(&group->channels, chan);
	pthread_mutex_unlock(&group->groupMutex);

	return chanNode;
}

struct link_Node *grp_getChannel(struct link_Node *groupNode, char *name){
	struct grp_Group *group = groupNode->data;
	if(group == NULL)
		return NULL;

	struct link_Node *node;
	struct chan_Channel *channel;
	pthread_mutex_lock(&group->groupMutex);
	
	for(node = group->channels.head; node != NULL; node = node->next){
		channel = node->data;
		pthread_mutex_lock(&channel->channelMutex);

		if(!strncmp(channel->name, name, fig_Configuration.chanNameLength)){
			pthread_mutex_unlock(&group->groupMutex);
			pthread_mutex_unlock(&channel->channelMutex);
			return node;
		}

		pthread_mutex_unlock(&channel->channelMutex);
	}

	pthread_mutex_unlock(&group->groupMutex);

	return NULL;
}
