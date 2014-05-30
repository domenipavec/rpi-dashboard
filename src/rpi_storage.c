/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "rpi_network.h"

#include "rpi_modules.h"

#include "packages/json/json.h"

#include <mntent.h>
#include <sys/statvfs.h>

json_t * rpi_storage_get_list(duda_request_t *dr)
{
    struct mntent *ent;
    FILE *f;
    json_t *array;
    json_t *mount_object;
    char *full_path;
    struct statvfs svfs;
    double fragment_size, size, used, available, use;
    
    array = json->create_array();
    
    f = setmntent("/proc/mounts", "r");
    if (f == NULL) {
        return array;
    }
    
    while (NULL != (ent = getmntent(f))) {
        mount_object = json->create_object();
        json->add_to_array(array, mount_object);
        
        /* resolve symlinks in device names */
        full_path = NULL;
        if (ent->mnt_fsname[0] == '/') {
            full_path = realpath(ent->mnt_fsname, full_path);
        }
        if (full_path != NULL) {
            json->add_to_object(mount_object, "device", json->create_string(full_path));
            free(full_path);
        }
        else {
            json->add_to_object(mount_object, "device", json->create_string(ent->mnt_fsname));
        }
        json->add_to_object(mount_object, "mount", json->create_string(ent->mnt_dir));
        json->add_to_object(mount_object, "filesystem", json->create_string(ent->mnt_type));
        
        /* try to get usage data */
        if ((statvfs(ent->mnt_dir, &svfs) == 0) && (svfs.f_blocks != 0)) {
            fragment_size = (double)svfs.f_frsize;
            size = fragment_size*((double)svfs.f_blocks);
            used = size - fragment_size*((double)svfs.f_bfree);
            available = fragment_size*((double)svfs.f_bavail);
            use = (size - available)/size;
            
            json->add_to_object(mount_object, "size", json->create_number(size));
            json->add_to_object(mount_object, "used", json->create_number(used));
            json->add_to_object(mount_object, "available", json->create_number(available));
            json->add_to_object(mount_object, "use", json->create_number(use));
        }
    }
    
    endmntent(f);
    
    return array;
}

json_t * rpi_storage_get_throughput(duda_request_t *dr)
{
    FILE *f;
    json_t *object;
    json_t *dobject;
    char device[20];
    double read, written;
    
    object = json->create_object();
    
    f = fopen("/proc/diskstats", "r");
    if (f == NULL) {
        return object;
    }
    
    while (3 == fscanf(f, " %*d %*d %19s %*d %*d %lf %*d %*d %*d %lf %*d %*d %*d %*d", device, &read, &written)) {
        if (read == 0.0 && written == 0.0) {
            continue;
        }
        
        read *= 512.0;
        written *= 512.0;
        
        dobject = json->create_object();
        json->add_to_object(object, device, dobject);
        
        json->add_to_object(dobject, "total", json->create_number(read + written));
        json->add_to_object(dobject, "read", json->create_number(read));
        json->add_to_object(dobject, "written", json->create_number(written));
    }
    
    fclose(f);
    
    return object;
}

/* register and initialize module */
void rpi_storage_init(void)
{
    rpi_module_t *module = rpi_modules_module_init("storage", NULL);
    
    rpi_modules_value_init("list", rpi_storage_get_list, &(module->values_head.values));
    rpi_modules_value_init("throughput", rpi_storage_get_throughput, &(module->values_head.values));
}