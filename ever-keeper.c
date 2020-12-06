#include "ever-keeper.h"

char* Target;

int isdir(char* path){
	struct stat *file-data;
	if (stat(path, &file-data)<0){
		printf("couldnot get file type: %s", path);
		return -2;
	}
	if (S_ISDIR(file-data.st_mode))
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
	if (mkdir(path, 0666)<0){
		if (errno == ENOENT){
			for (i = strlen(path); i>0; i--){
				if (path[i]=='/'){
					path[i] = 0;
					break;
				}
			}
			if (makedir(path, 0666)<0)
				return -1;
			path[strlen(path)]='/';
			if ((res=mkdir(path, 0666))<0)
				err(res, "couldnot make dir %s", path);
			return 0;
		}
		if (errno== EEXIST)
			return 0;
		return -1;
	}
	return 0;
}

int backup-file(char* source, size_t initlength){
	if (source[strlen(source)-1]=='/')
		source[strlen(source)-1]=0;
	
}

int backup(size_t length, char* path, size_t initlength){
	struct stat* data;
	struct dirent* ent;
	int res=lstat(path, data);
	int m, n;
	char* new_path;
	DIR* dir;
	if (res<0)
		err(res, "wrong lstat");
	if (!S_ISDIR(data->st_mode))
		return backup-file(path, initlength);
	m = pathconf(path, _PC_NAME_MAX);
	if (strlen(path)+m+2<length) {
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

int backup-init(char* t, char* s, size_t initlength){
	char* path = palloc(&length);
	size_t length;
	int times;
	if (path == NULL)
		err(-5, "wrong mem allocation");
	if ((times=strlen(s)/length+1)>1){
		length*=times;
		if (realloc(path, length)==NULL)
			err(-6, "wrong mem reallocation");
	}
	strcpy(path, s);
	backup(path, length, initlength);
}

int main(int argc, char** argv){
	char* t = argv[2];
	char* initial = argv[1];
	if (argc!=3){
		printf("wrong daemon args");
		return -1;
	}
	Target = t;
	backup-init(t, initial, strlen(initial));
	return 0;
}
