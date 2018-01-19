export ANDROID_NDK_ROOT="/opt/android-ndk-r14b"
export ANDROID_HOST_SYSTEM="linux-x86_64"
scons target=arm-android-linux build_config=Release android_toolchain=arm-linux-androideabi-4.9 android_cross_prefix=arm-linux-androideabi android_platform=android-22
