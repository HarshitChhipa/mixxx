// ratecontrol.h
// Created 7/4/2009 by RJ Ryan (rryan@mit.edu)

#ifndef RATECONTROL_H
#define RATECONTROL_H

#include <QObject>

#include "preferences/usersettings.h"
#include "engine/enginecontrol.h"
#include "engine/sync/syncable.h"

const int RATE_TEMP_STEP = 500;
const int RATE_TEMP_STEP_SMALL = RATE_TEMP_STEP * 10.;
const int RATE_SENSITIVITY_MIN = 100;
const int RATE_SENSITIVITY_MAX = 2500;

class BpmControl;
class Rotary;
class ControlTTRotary;
class ControlObject;
class ControlPotmeter;
class ControlPushButton;
class ControlProxy;
class EngineChannel;
class PositionScratchController;

// RateControl is an EngineControl that is in charge of managing the rate of
// playback of a given channel of audio in the Mixxx engine. Using input from
// various controls, RateControl will calculate the current rate.
class RateControl : public EngineControl {
    Q_OBJECT
public:
    RateControl(QString group, UserSettingsPointer pConfig);
    ~RateControl() override;

    // Enumerations which hold the state of the pitchbend buttons.
    // These enumerations can be used like a bitmask.
    enum RATERAMP_DIRECTION {
        RATERAMP_NONE = 0,  // No buttons are held down
        RATERAMP_DOWN = 1,  // Down button is being held
        RATERAMP_UP = 2,    // Up button is being held
        RATERAMP_BOTH = 3   // Both buttons are being held down
    };

    enum class RampMode {
        Stepping = 0, // pitch takes a temporary step up/down a certain amount
        Linear = 1 // pitch moves up/down in a progresively linear fashion
    };

    void setBpmControl(BpmControl* bpmcontrol);

    // Returns the current engine rate.  "reportScratching" is used to tell
    // the caller that the user is currently scratching, and this is used to
    // disable keylock.
    double calculateSpeed(double baserate, double speed, bool paused,
                         int iSamplesPerBuffer, bool* pReportScratching,
                         bool* pReportReverse);
    double calcRateRatio() const;

    // Set rate change when temp rate button is pressed
    static void setTemporaryRateChangeCoarseAmount(double v);
    static double getTemporaryRateChangeCoarseAmount();
    // Set rate change when temp rate small button is pressed
    static void setTemporaryRateChangeFineAmount(double v);
    static double getTemporaryRateChangeFineAmount();
    // Set rate change when perm rate button is pressed
    static void setPermanentRateChangeCoarseAmount(double v);
    static double getPermanentRateChangeCoarseAmount();
    // Set rate change when perm rate small button is pressed
    static void setPermanentRateChangeFineAmount(double v);
    static double getPermanentRateChangeFineAmount();
    // Set Rate Ramp Mode
    static void setRateRampMode(RampMode mode);
    static RampMode getRateRampMode();
    // Set Rate Ramp Sensitivity
    static void setRateRampSensitivity(int);
    static int getRateRampSensitivity();
    void notifySeek(double dNewPlaypos, bool adjustingPhase) override;

  public slots:
    void slotReverseRollActivate(double);
    void slotControlRatePermDown(double);
    void slotControlRatePermDownSmall(double);
    void slotControlRatePermUp(double);
    void slotControlRatePermUpSmall(double);
    void slotControlRateTempDown(double);
    void slotControlRateTempDownSmall(double);
    void slotControlRateTempUp(double);
    void slotControlRateTempUpSmall(double);
    void slotControlFastForward(double);
    void slotControlFastBack(double);

  private:
    void processTempRate(const int bufferSamples);
    double getJogFactor() const;
    double getWheelFactor() const;
    SyncMode getSyncMode() const;

    // Set rate change of the temporary pitch rate
    void setRateTemp(double v);
    // Add a value to the temporary pitch rate
    void addRateTemp(double v);
    // Subtract a value from the temporary pitch rate
    void subRateTemp(double v);
    // Reset the temporary pitch rate
    void resetRateTemp(void);
    // Get the 'Raw' Temp Rate
    double getTempRate(void);

    // Values used when temp and perm rate buttons are pressed
    static ControlValueAtomic<double> m_dTemporaryRateChangeCoarse;
    static ControlValueAtomic<double> m_dTemporaryRateChangeFine;
    static ControlValueAtomic<double> m_dPermanentRateChangeCoarse;
    static ControlValueAtomic<double> m_dPermanentRateChangeFine;

    ControlPushButton* m_pButtonRateTempDown;
    ControlPushButton* m_pButtonRateTempDownSmall;
    ControlPushButton* m_pButtonRateTempUp;
    ControlPushButton* m_pButtonRateTempUpSmall;

    ControlPushButton* m_pButtonRatePermDown;
    ControlPushButton* m_pButtonRatePermDownSmall;
    ControlPushButton* m_pButtonRatePermUp;
    ControlPushButton* m_pButtonRatePermUpSmall;

    ControlObject* m_pRateDir;
    ControlObject* m_pRateRange;
    ControlPotmeter* m_pRateSlider;
    ControlPotmeter* m_pRateSearch;
    ControlPushButton* m_pReverseButton;
    ControlPushButton* m_pReverseRollButton;
    ControlObject* m_pBackButton;
    ControlObject* m_pForwardButton;

    ControlTTRotary* m_pWheel;
    ControlObject* m_pScratch2;
    PositionScratchController* m_pScratchController;

    ControlPushButton* m_pScratch2Enable;
    ControlObject* m_pJog;
    ControlObject* m_pVCEnabled;
    ControlObject* m_pVCScratching;
    ControlObject* m_pVCMode;
    ControlObject* m_pScratch2Scratching;
    Rotary* m_pJogFilter;

    ControlObject* m_pSampleRate;

    // For Master Sync
    BpmControl* m_pBpmControl;

    ControlProxy* m_pSyncMode;
    ControlProxy* m_pSlipEnabled;

    // The current rate ramping direction. Only holds the last button pressed.
    int m_ePbCurrent;
    //  The rate ramping buttons which are currently being pressed.
    int m_ePbPressed;

    // This is true if we've already started to ramp the rate
    bool m_bTempStarted;
    // Set the Temporary Rate Change Mode
    static RampMode m_eRateRampMode;
    // The Rate Temp Sensitivity, the higher it is the slower it gets
    static int m_iRateRampSensitivity;
    // Factor applied to the deprecated "wheel" rate value.
    static const double kWheelMultiplier;
    // Factor applied to jogwheels when the track is paused to speed up seeking.
    static const double kPausedJogMultiplier;
    // Temporary pitchrate, added to the permanent rate for calculateRate
    double m_tempRateRatio;
    // Speed for temporary rate change
    double m_dRateTempRampChange;
};

#endif /* RATECONTROL_H */
