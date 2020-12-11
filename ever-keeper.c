#include "ever-keeper.h"

/*содержит слэш!*/
char* Target;

int isdir(char* path){
	struct stat file_data;
	if (lstat(path, &file_data)<0){
		printf("couldnot get file type: %s", path);
		return -2;
	}
	if (S_ISDIR(file_data.st_mode))
		return 1;
	else
		return 0;
}

char* palloc(size_t* size){
	char *ptr;
	size_t s;
	if (pathmax==0){
		if ((pathmax=pathconf("/", _PC_PATH_MAX))<0)
			pathmax = PATH_MAX_HOPE;
		else
			pathmax++;
	}
	s = pathmax;
	ptr = malloc(s);
	if (size!=NULL)
		*size = s;
	return (char*)ptr;
}

/*без слэша!*/
int makedir(char* path){
	int i, res;
	if (mkdir(path, 0777)<0){
		if (errno == ENOENT){
			for (i = strlen(path); i>0; i--){
				if (path[i]=='/'){
					path[i] = 0;
					break;
				}
			}
			if (makedir(path)<0)
				return -1;
			path[strlen(path)]='/';
			if ((res=mkdir(path, 0777))<0)
				err(res, "couldnot make dir %s", path);
			return 0;
		}
		if (errno== EEXIST)
			return 0;
		err(res, "couldnot make dir %s", path);

	}
	return 0;
}

int backup_file(char* source, size_t initlength){
	struct stat datas;
	struct stat datat;
	char* targetPath=palloc(NULL);
	int i, fdf, fdt, res=0;
	strcpy(targetPath, Target);
	if (source[strlen(source)-1]=='/')
		source[strlen(source)-1]=0;
	strcat(targetPath, &source[initlength+1]);
	strcat(targetPath, ".gz");
	for (i=strlen(targetPath); i>0; i--)
		if (targetPath[i]=='/'){
			targetPath[i]=0;
			break;
		}
	if (makedir(targetPath)<0)
		exit(-1);
	targetPath[strlen(targetPath)]='/';
	if (lstat(targetPath, &datat)<0){
		if (errno==ENOENT)
			datat.st_mtim.tv_sec = 0;
		else{
			warn("couldnot get data on file %s", targetPath);
			return -1;
		}
	}
	if (lstat(source, &datas)<0){
		warn("couldnot get data on file %s", source);
		return -1;
	}
	targetPath[strlen(targetPath)-3]=0;
	if (datas.st_mtim.tv_sec>datat.st_mtim.tv_sec){
		if ((fdf = open(source, O_RDWR))<0){
			warn("Couldnot open source file %s", source);
			res=-1;
		}
		if ((fdt = open(targetPath, O_RDWR|O_CREAT, 0666))<0){
			warn("Couldnot open target file %s", targetPath);
			res = -1;
		}
		if ((res=sendfile(fdt, fdf, NULL, datas.st_size))<0){
			warn("couldnot copy data from %s to%s", source, targetPath);
			res=-1;
		}
		close(fdf);
		close(fdt);
		if (res<0){
			unlink(targetPath);
			return res;
		}
		if (fork()==0){
			if ((execlp("gzip", "gzip", "--best", targetPath, (char*)0))<0)
				warn("couldnot gzip file %s", targetPath);
		}
	}
	return res;
}

int backup(char* path, size_t length, size_t initlength){
	struct stat data;
	struct dirent* ent;
	int res=lstat(path, &data);
	int m, n;
	char* new_path;
	DIR* dir;
	if (res<0)
		err(res, "wrong lstat");
	if (!S_ISDIR(data.st_mode))
		return backup_file(path, initlength);
	m = pathconf(path, _PC_NAME_MAX);
	if (strlen(path)+m+2>length) {
		length=strlen(path)+m+3;
		if (realloc(path, length)==NULL)
			err(-1, "wrong realloc");
	}
	if ((dir=opendir(path))==NULL){
		warn("couldnot open dir %s", path);
		return 0;
	}
	n=strlen(path);
	path[n]='/';
	path[n+1]=0;
	if (!strcmp(Target, path)){
		warnx("Skipping backup folder");
		return 0;
	}
	while ((ent=readdir(dir))!=NULL){
		if (strcmp(".", ent->d_name)==0||strcmp("..", ent->d_name)==0)
			continue;
		strcpy(&path[n+1], ent->d_name);
		backup(path, length, initlength);
		path[n+1]=0;
	}
	closedir(dir);
	return 0;
}

int backup_init(char* t, char* s, size_t initlength){
	size_t length;
	char* path = palloc(&length);
	int times;
	if (t[strlen(t)-1]!='/'){
		t[strlen(t)]='/';
		t[strlen(t)+1]=0;
	}
	Target = palloc(NULL);
	strcpy(Target, t);
	if (path == NULL)
		err(-5, "wrong mem allocation");
	if ((times=strlen(s)/length+1)>1){
		length*=times;
		if (realloc(path, length)==NULL)
			err(-6, "wrong mem reallocation");
	}
	strcpy(path, s);
	return backup(path, length, initlength);
}

void daemonize(){
	pid_t pid;
	umask(0);
	if ((pid=fork())>0)
		exit(0);
	if (pid<0)
		err(-1, "Daemonization failure");
	setsid();
	if ((pid=fork())>0)
		exit(0);
	if (pid<0)
		err(-1, "Daemonization failure");
	chdir("/");
}

int main(int argc, char** argv){
	char* t = palloc(NULL);
	int log = open("logfile", O_WRONLY|O_CREAT|O_SYNC|O_TRUNC, 0666);
	time_t prev = 0;
	char* initial = palloc(NULL);
	size_t i;
	dup2(log, STDERR_FILENO);
#ifdef DAEMON
	daemonize();
#endif
	strcpy(t, argv[2]);
	strcpy(initial, argv[1]);
	if (t==NULL||initial==0)
		err(-1, "wrong mem allocation");
	if (argc!=3){
		printf("wrong daemon args");
		return -1;
	}
	if (initial[strlen(initial)-1]=='/')
		initial[strlen(initial)-1]='\0';
	i = strlen(initial);
	while (1)
		if (time(NULL)>prev+60){
			prev = time(NULL);
			backup_init(t, initial, i);
		}
		else
			sleep(60);
	return 0;
}
