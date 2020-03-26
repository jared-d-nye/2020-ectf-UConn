---
title: eCTF 2020
created: '2020-01-29T23:01:58.667Z'
modified: '2020-01-30T00:02:44.699Z'
---

# eCTF 2020
## Process Steps
1. Generate region secrets.
   `./createRegions`
2. Generate user secrets.
   `./createUsers`
3. Protect audio files.
   `./protectSong`
4. Create device. Provisioned for specific regions and users.
   `./createDevice`
5. Create Vivado project.
   `./buildDevice`
6. Create miPod binaries
   `./packageDevice`
7. Push binaries to SD card
   `./deployDevice`

## Operation
### ./createRegions
Generates `region.secret` with the following structure:
`{"region": region_id}`

### ./createUsers
Generates `user.secret` with the following structure:
`{"user": {"pin": "00000000", "id": user_id}}`

### ./protectSong
Provisions a `.wav` file for specific regions and users. This will perform the following operations:
1. `createMetadata`
   Generates byte string of the form:
   `METADATA_LENGTH(1B)/ownerID(1B)/REGION_LEN(1B)/USER_LEN(1B)/REGIONID1(1B)/REGIONID2 (1B)/.../opt. parity`
   Note that the metadata must be of even length because samples are 2B and ARM requires type alignment
2. `ProtectedSong`
   Creates an object representing the protected song. This generates a `song.drm` from `song.wav` through prepending the generated metadata to the original song data.

### ./createDevice
Generates device secrets using `region.secret` and `user.secret`. The file generated is `device_secrets.h`, containing a C version of the region and user secret info.

### ./packageDevice
MITRE-provided tool. Not developed by our team.

### ./buildDevice
Generates Vivado project. Steps are well documented in the source file.

### ./deployDevice
Writes data to the SD card using the following operations:
* `setup_sdcard`
  Formats and partitions the SD card into `BOOT` and `root` partitions.
* `copy_boot`
  Copies all boot files to the `BOOT` partition
* `copy_audio`
  Copies all audio files to the `root` partition
* `copy_mipod`
  Copies miPod application to the `root` partition
* `copy_imageub`
  Copies image.ub to the `BOOT` partition

