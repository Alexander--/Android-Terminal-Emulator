package jackpal.androidterm.libtermexec;

import android.os.ParcelFileDescriptor;

// see also:
// the (clumsy) way to handle object inheritance with Binder:
// https://kevinhartman.github.io/blog/2012/07/23/inheritance-through-ipc-using-aidl-in-android/
// some (possibly outdated) notes on preserving backward compatibility:
// https://stackoverflow.com/questions/18197783/android-aidl-interface-parcelables-and-backwards-compatibility
interface ITerminal {
    // accepts the file descriptor, obtained by opening /dev/ptmx
    void startHandling(in ParcelFileDescriptor pseudoTerminalMultiplexerFd);
}
