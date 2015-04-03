/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package android.hardware.fingerprint;

import android.os.Bundle;
import android.hardware.fingerprint.IFingerprintServiceReceiver;
import android.hardware.fingerprint.Fingerprint;
import java.util.List;

/**
 * Communication channel from client to the fingerprint service.
 * @hide
 */
interface IFingerprintService {
    // Authenticate the given sessionId with a fingerprint
    void authenticate(IBinder token, long sessionId, int groupId, int flags);

    // Start fingerprint enrollment
    void enroll(IBinder token, int groupId, int flags);

    // Any errors resulting from this call will be returned to the listener
    void remove(IBinder token, int fingerId, int groupId);

    // Rename the fingerprint specified by fingerId and groupId to the given name
    void rename(int fingerId, int groupId, String name);

    // Get a list of enrolled fingerprints in the given group.
    List<Fingerprint> getEnrolledFingerprints(int groupId);

    // Register listener for an instance of FingerprintManager
    void addListener(IBinder token, IFingerprintServiceReceiver receiver, int userId);

    // Unregister listener for an instance of FingerprintManager
    void removeListener(IBinder token, IFingerprintServiceReceiver receiver);

    // Determine if HAL is loaded and ready
    boolean isHardwareDetected(long deviceId);

    // Gets the number of hardware devices
    // int getHardwareDeviceCount();

    // Gets the unique device id for hardware enumerated at i
    // long getHardwareDevice(int i);

}