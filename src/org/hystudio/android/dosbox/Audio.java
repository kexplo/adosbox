/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2011 Sam Lantinga
    Java source code (C) 2009-2011 Sergii Pylypenko

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
*/

package org.hystudio.android.dosbox;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;

class AudioThread {

	private MainActivity mParent;
	private AudioTrack mAudio;
	private byte[] mAudioBuffer;
	private int mVirtualBufSize;

	public AudioThread(MainActivity parent) {
		mParent = parent;
		mAudio = null;
		mAudioBuffer = null;
		nativeAudioInitJavaCallbacks();
	}

	public int fillBuffer() {
		if (mParent.isPaused()) {
			try {
				Thread.sleep(200);
			} catch (InterruptedException e) {
			}
		} else {
			// if( Globals.AudioBufferConfig == 0 ) // Gives too much spam to
			// logcat, makes things worse
			// mAudio.flush();

			mAudio.write(mAudioBuffer, 0, mVirtualBufSize);
		}

		return 1;
	}

	public int initAudio(int rate, int channels, int encoding, int bufSize) {
		if (mAudio == null) {
			channels = (channels == 1) ? AudioFormat.CHANNEL_CONFIGURATION_MONO
					: AudioFormat.CHANNEL_CONFIGURATION_STEREO;
			encoding = (encoding == 1) ? AudioFormat.ENCODING_PCM_16BIT
					: AudioFormat.ENCODING_PCM_8BIT;

			if (AudioTrack.getMinBufferSize(rate, channels, encoding) > bufSize)
				bufSize = AudioTrack.getMinBufferSize(rate, channels, encoding);

			mVirtualBufSize = bufSize;

			if (Globals.AudioBufferConfig != 0) { // application's choice - use
													// minimal buffer
				bufSize = (int) ((float) bufSize * (((float) (Globals.AudioBufferConfig - 1) * 2.5f) + 1.0f));
				mVirtualBufSize = bufSize;
			}
			mAudioBuffer = new byte[bufSize];

			mAudio = new AudioTrack(AudioManager.STREAM_MUSIC, rate, channels,
					encoding, bufSize, AudioTrack.MODE_STREAM);
			mAudio.play();
		}
		return mVirtualBufSize;
	}

	public byte[] getBuffer() {
		return mAudioBuffer;
	}

	public int deinitAudio() {
		if (mAudio != null) {
			mAudio.stop();
			mAudio.release();
			mAudio = null;
		}
		mAudioBuffer = null;
		return 1;
	}

	public int initAudioThread() {
		// Make audio thread priority higher so audio thread won't get underrun
		Thread.currentThread().setPriority(Thread.MAX_PRIORITY);
		return 1;
	}

	public int pauseAudioPlayback() {
		if (mAudio != null) {
			mAudio.pause();
			return 1;
		}
		return 0;
	}

	public int resumeAudioPlayback() {
		if (mAudio != null) {
			mAudio.play();
			return 1;
		}
		return 0;
	}

	private native int nativeAudioInitJavaCallbacks();
}
