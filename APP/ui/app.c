#include "app.h"

app_mgr_t* app_mgr_inst(void){
	static app_mgr_t app_mgr;
	if (!app_mgr.app_mgr_inited){
		app_mgr.app_registered = 0;
		app_mgr.app_head = NULL;
		app_mgr.app_mgr_inited = 1;
	}
	return &app_mgr;
}

void start_app_mgr(void){
	static u8 started = 0;
	if (!started){
		app_mgr_register((app_t*)create_settings_app());
		app_mgr_register((app_t*)create_calc_app());
		started = 1;
	} else {
		printf("App manager already started!\r\n");
	}
}

void app_mgr_register(app_t* my_app){
	if(my_app == NULL){
		printf("Cannot register NULL app!\r\n");
		return;
	}
	if(my_app->is_registered){
		printf("App already registered!\r\n");
		return;
	}
	my_app->is_registered = 1;
	(app_mgr_inst()->app_registered)++;

	app_t* tmp = app_mgr_inst()->app_head;
	if(NULL == tmp) {
		app_mgr_inst()->app_head = my_app;
		return;
	}
	while(NULL != tmp->next_app){
		tmp = tmp->next_app;
	}
	tmp->next_app = my_app;
	
}

void app_mgr_unreg(app_t* my_app){
	// find app
	app_t* tmp = app_mgr_inst()->app_head;
	app_t* prev = NULL;
	while(my_app != tmp){
		prev = tmp;
		tmp = tmp->next_app;
	}
	// list remove
	if(NULL == prev){
		app_mgr_inst()->app_head = tmp->next_app;
	} else {
		prev->next_app = tmp->next_app;
	}
	tmp->next_app = NULL;
}


static void app_t_exit_t(app_t* self){

}


