# Care Companion – AI Assisted Elderly Healthcare Stick

![Care Companion](images/care_companion.jpg)

## Overview

Care Companion is an AI-assisted smart walking stick designed to improve medication compliance, safety, and independence for elderly individuals. The device combines computer vision, voice guidance, medicine verification, and health assistance features into a portable form factor that can be attached directly to a walking stick.

One of the most common problems faced by elderly people is forgetting medication schedules or accidentally taking the wrong medicine. Care Companion addresses this issue by utilizing HuskyLens AI Vision and AprilTag recognition to verify medicines before consumption while providing real-time voice instructions and reminders.

The system acts as a personal healthcare assistant that can guide users throughout their medication routine without requiring smartphones, internet connectivity, or caregiver intervention.

---

## Key Features

### Smart Medicine Reminder

* Scheduled medication alerts
* Audio reminder notifications
* Missed medicine detection
* Medication verification workflow

### AI-Based Medicine Verification

* HuskyLens vision-based detection
* AprilTag recognition system
* Unique identification for each medicine
* Wrong medicine warning
* Correct medicine confirmation

### Voice Guided Assistance

* Step-by-step audio instructions
* Reminder notifications
* Verification feedback
* Error notifications
* Elderly-friendly interaction

### Portable Healthcare Assistant

* Walking-stick-mounted design
* Lightweight architecture
* Battery powered operation
* Suitable for home use

### Safety Features

* Medication error prevention
* Visual and audio feedback
* Verification before intake
* Reduced dependency on caregivers

---

# Why AprilTag?

Traditional medicine reminder systems only remind users when to take medicine. They cannot verify whether the correct medicine is actually being consumed.

Care Companion solves this problem by attaching a unique AprilTag marker to each medicine container.

Example:

| Medicine                | AprilTag ID |
| ----------------------- | ----------- |
| Blood Pressure Medicine | ID 01       |
| Diabetes Medicine       | ID 02       |
| Vitamin Supplement      | ID 03       |
| Pain Relief Medicine    | ID 04       |

When the user presents the medicine bottle to the camera:

1. HuskyLens detects the AprilTag.
2. The tag ID is extracted.
3. The ID is compared against the medication schedule.
4. The system confirms whether the medicine is correct.
5. Audio feedback is provided.

Example:

```text
Reminder: Please take your blood pressure medicine.

Detected Tag: ID 01

Medicine Verified.

You may now take the medicine.
```

If the wrong medicine is presented:

```text
Warning.

Incorrect medicine detected.

Please check your medication.
```

This significantly reduces medication-related mistakes.

---

## System Architecture

```text
+----------------------+
| Medication Schedule |
+----------+-----------+
           |
           v
+----------------------+
| Reminder Generator  |
+----------+-----------+
           |
           v
+----------------------+
| Voice Notification  |
+----------+-----------+
           |
           v
+----------------------+
| HuskyLens Camera    |
+----------+-----------+
           |
           v
+----------------------+
| AprilTag Detection  |
+----------+-----------+
           |
           v
+----------------------+
| Verification Engine |
+----------+-----------+
           |
           v
+----------------------+
| User Feedback       |
+----------------------+
```

---

## Hardware Components

### Processing Unit

* ESP32 Development Board

### Vision System

* HuskyLens AI Vision Sensor
* AprilTag Marker Set

### User Interface

* TFT Display
* Status Indicators
* Audio Feedback System

### Audio System

* Speaker Module
* Voice Prompt Storage

### Storage

* MicroSD Card Module

### Communication

* UART Interface
* I2C Interface

### Power System

* Rechargeable Battery
* Portable Operation

### Mechanical Structure

* Walking Stick Mount
* Compact Electronics Enclosure

---

## Software Features

### Reminder Management

* Medicine scheduling
* Alert generation
* Daily medication tracking

### Vision Processing

* AprilTag detection
* Medicine identification
* Verification logic

### Voice Assistance

* Audio playback
* User guidance
* Verification announcements

### System Management

* Time tracking
* Schedule handling
* Device monitoring

---

## Use Case Example

### Morning Medication

08:00 AM

The system announces:

```text
Good Morning.

It is time to take your blood pressure medicine.
```

The user presents the medicine bottle.

The HuskyLens camera scans the attached AprilTag.

If correct:

```text
Medicine Verified.

Please take one tablet.
```

If incorrect:

```text
Warning.

This is not the scheduled medicine.
```

---

## Applications

### Elderly Care

* Independent living support
* Medication management

### Assisted Living Facilities

* Resident healthcare support
* Reduced caregiver workload

### Smart Healthcare

* Embedded healthcare systems
* AI-assisted medical devices

### Research and Education

* Human-Computer Interaction
* AI Vision Systems
* Healthcare Robotics

---

## Repository Structure

```text
.
├── Care_companion.ino
├── images
│   ├── Care_Companion.jpg
│   ├── System_Diagram.png
│   ├── Hardware_Setup.png
│   └── AprilTag_Examples.png
├── audio
│   ├── Reminder_Prompts
│   ├── Warning_Prompts
│   └── Verification_Prompts
├── docs
│   ├── Circuit_Diagram.pdf
│   └── User_Manual.pdf
└── README.md
```

---

## Future Improvements

### Healthcare Expansion

* Heart rate monitoring
* Blood oxygen monitoring
* Fall detection

### Connectivity

* WiFi synchronization
* Mobile application
* Cloud dashboard

### Caregiver Support

* Remote notifications
* Medication history reports
* Compliance analytics

### AI Improvements

* Pill shape recognition
* Medicine package classification
* Voice command support

### Emergency Features

* SOS button
* GPS tracking
* Emergency contact notifications

---

## Demonstration

The system demonstrates how low-cost embedded AI can be integrated into assistive healthcare devices to improve medication compliance and independence for elderly users.

By combining HuskyLens AI Vision, AprilTag verification, and voice-guided interaction, Care Companion provides a practical solution to one of the most common challenges faced by aging populations.

---

## Educational Value

This project demonstrates:

* Embedded Systems Design
* Human-Robot Interaction
* AI Vision Applications
* Healthcare Technology
* Assistive Device Development
* ESP32 Programming
* Computer Vision Integration
* Medical Safety Systems

---

## License

This project is released for educational, research, healthcare prototyping, and non-commercial development purposes.
