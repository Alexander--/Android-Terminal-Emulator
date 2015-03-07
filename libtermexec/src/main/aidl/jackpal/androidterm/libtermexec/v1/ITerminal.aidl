package jackpal.androidterm.libtermexec.v1;

import android.content.IntentSender;
import android.os.ParcelFileDescriptor;
import android.os.ResultReceiver;

// see also:
// the (clumsy) way to handle object inheritance with Binder:
// https://kevinhartman.github.io/blog/2012/07/23/inheritance-through-ipc-using-aidl-in-android/
// some (possibly outdated) notes on preserving backward compatibility:
// https://stackoverflow.com/questions/18197783/android-aidl-interface-parcelables-and-backwards-compatibility
/**
  * An interface for interacting with Terminal implementation.
  *
  * The version of the interface is encoded in Intent action and the AIDL package name. New versions
  * of this interface may be implemented in future. Those versions will be made available
  * in separate packages and older versions will continue to work.
  */
interface ITerminal {
    /**
      * Start a new Terminal session. A session will remain hosted by service, that provides binding,
      * but no gurantees of process pesistence as well as stability of connection are made. You
      * should keep your ParcelFileDescriptor around and allow ServiceConnection to call this method
      * again, when reconnection happens, in case service hosting the session is killed by system.
      *
      * You can have only one simultaneously running session per binding. Initiate one more binding
      * or stop previous session to start another one. Attempts to start a session on binding, which
      * still have active session are ignored.
      *
      * @param pseudoTerminalMultiplexerFd file descriptor, obtained by opening /dev/ptmx.
      *
      * @return IntentSender, that can be used to start corresponding Terminal Activity.
      */
    IntentSender startSession(in ParcelFileDescriptor pseudoTerminalMultiplexerFd);

    /**
      * Allows caller to be notified of terminal session events. Multiple callbacks can be made.
      * Multiple calls can happen on each, and new call types can be introduced, so prepare to
      * ignore unknown Intent actions and extras.
      *
      * So far only notifications about session end (code 0) are supported. This notification is
      * issued after abovementioned file descriptor is closed and the session is ended from
      * Terminal's standpoint.
      *
      * @param a callback
      */
    void registerSessionCallback(in ResultReceiver callback);
}
