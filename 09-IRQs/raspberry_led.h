/*
 * Raspberry Bare Metal
 * Copyright (C) 2014  Federico "MrModd" Cosentino (http://mrmodd.it/)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RASPBERRY_H
#error You should not include sub-header files
#endif

#define GPIO_LED 16
#define GPIO_LED_SEL_OFFSET 18

#define LED_ON GPIO0_SET_LOW(GPIO_LED)

#define LED_OFF GPIO0_SET_HIGH(GPIO_LED)

#define LED_TOGGLE GPIO0_TOGGLE_BIT(GPIO_LED)
