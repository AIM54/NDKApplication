package com.bian.myapplication.service;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.IBinder;

import com.bian.myapplication.R;

import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;

public class SubmitService extends Service {
    /**
     * 通知Id
     */
    public final static int UPLOADING_BENZ_ORDER_NOTIFYCATION_ID = 100;
    private NotificationCompat.Builder mBuilder;
    private String mNotificationTitle,mNotificationContent;

    public SubmitService() {
    }

    @Override
    public void onCreate() {
        super.onCreate();
        mNotificationTitle = getString(R.string.app_label);
        mNotificationContent = getString(R.string.benz_car_is_running);
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }

    /**
     * 打开前台进程
     */
    private void setUpNotication() {
        if (mBuilder == null) {
            createNotifycation();
            startForeground(UPLOADING_BENZ_ORDER_NOTIFYCATION_ID, mBuilder.build());
        } else {
            Notification notifycation = mBuilder
                    .setContentText(mNotificationContent)
                    .setContentTitle(mNotificationTitle)
                    .setWhen(System.currentTimeMillis())
                    .build();
            NotificationManagerCompat.from(this).notify(UPLOADING_BENZ_ORDER_NOTIFYCATION_ID, notifycation);
        }
    }


    /**
     * 创建notifycation 保活
     *
     * @return
     */
    private void createNotifycation() {
        String CHANNEL_ONE_ID = getPackageName();
        String CHANNEL_ONE_NAME = getString(R.string.benz_car_is_running);
        NotificationChannel notificationChannel = null;
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
            notificationChannel = new NotificationChannel(CHANNEL_ONE_ID,
                    CHANNEL_ONE_NAME, NotificationManager.IMPORTANCE_LOW);
            notificationChannel.setShowBadge(true);
            notificationChannel.enableLights(false);
            notificationChannel.enableVibration(false);
            notificationChannel.setLockscreenVisibility(Notification.VISIBILITY_PUBLIC);
            NotificationManager manager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
            manager.createNotificationChannel(notificationChannel);
        }
        Bitmap bmp = BitmapFactory.decodeResource(getResources(), R.mipmap.ic_launcher);
        PendingIntent pendingIntent = PendingIntent.getBroadcast(this, 0, new Intent("click"), PendingIntent.FLAG_UPDATE_CURRENT);
        mBuilder = new NotificationCompat.Builder(this, CHANNEL_ONE_ID);
        mBuilder.setDefaults(Notification.DEFAULT_LIGHTS)
                .setPriority(NotificationManagerCompat.IMPORTANCE_MIN)
                .setVibrate(null)
                .setContentIntent(pendingIntent)
                .setContentText(mNotificationContent)
                .setContentTitle(mNotificationTitle)
                .setOnlyAlertOnce(true)
                .setSmallIcon(R.mipmap.ic_launcher)
                .setLargeIcon(bmp)
                .setWhen(System.currentTimeMillis())
                .setTicker(mNotificationContent);
    }

}
