typedef struct
{
  gchar *project_name;
  gchar *project_file_name;
  gchar *dir_makefile;
  gchar *dir_source;
  gchar *file_executable;


} t_tea_project;

t_tea_project *cur_tea_project;

GtkWidget *wnd_proj_props;


GtkWidget* create_proj_props_window (t_tea_project *p);


t_tea_project* tea_proj_new (void);
t_tea_project* tea_proj_clear_new (t_tea_project *p);
t_tea_project* tea_proj_open (gchar *name);
void tea_proj_save (t_tea_project *p);
void tea_proj_save_as (t_tea_project *p, gchar *name);
void tea_proj_free (t_tea_project *p);
void tea_proj_run (t_tea_project *p);



