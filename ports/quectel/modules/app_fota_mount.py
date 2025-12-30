import uos


class AppFotaPkgMount(object):
    __instance = None
    __mount_state = False
    __can_mount = False
    def __new__(cls, *args, **kwargs):
        if cls.__instance is None:
            cls.__instance = object.__new__(cls, *args, **kwargs)
        return cls.__instance

    def __init__(self):
        self.__fota_dir = "/fota"
    
    @property
    def mount_state(self):
        return self.__mount_state

    @property
    def can_mount(self):
        return self.__can_mount

    @property
    def fota_dir(self):
        if not self.can_mount:
            return ""
        else:
            return self.__fota_dir
                
    def mount_disk(self):    
        try:
            if not self.mount_state:
                fota_pkg = uos.VfsTemp("customer_temp_fs")
                uos.mount(fota_pkg, self.__fota_dir)
        except Exception as e:
            self.__mount_state = False
            self.__can_mount = False
        else:
            self.__mount_state = True
            self.__can_mount = True

    def umount_disk(self):
        if self.mount_state:
            uos.umount(self.fota_dir)
            self.__mount_state = False

    def get_fota_file_name(self, path):
        return self.fota_dir + path