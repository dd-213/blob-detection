# Vehicle detection by blob detection implementation 
> Implementation of blob detection algorithm for vehicle recognition on two-way roads.

## Table of Contents
* [General Info](#general-information)
* [Project Status](#project-status)
* [Room for Improvement](#room-for-improvement)
* [Acknowledgements](#acknowledgements)

## General Information
- implementation of blob detection (using findContours from opencv library)
- specifically used for vehicle recognition on two-way road
- may be broadened to other uses, as blob detection is implemented individually and relies on motion detection

## Project Status
Project is: _in progress_ 

- implemented blob detection
- succesfully tracks cars

## Room for Improvement
Possible improvement of existing parts:
- blob detection could be more isolated

To do:
- trucks (and any longer vehicles) are not yet supported
- futher changes needed for effective counting, allowing for momentary loss of tracking

## Acknowledgements
This projects is based on [this project](https://github.com/ahmetozlu/vehicle_counting).
Algorithm for blob detection was used as described in _Implementation of Motion detection and Tracking based 
on Mathematical Morpohology_ by Sheetal Balsara, Uday Joshi (2013)
