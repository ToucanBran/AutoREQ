# AutoREQ

In healthcare, most hospitals utilize supply closets to hold medical items for patient use. Nursing staff will come and go, taking
supplies as needed while employees from the Materials Management department will keep track of which items need replenishment. Because
item bin is assigned a minimum and maximum amount of supply, Materials Management staff knows about how much will need to be refilled.

A typical workflow for item use/replenishment is as follows:
  1.  Nursing staff will use items throughout the day
  2.  A distribution technician from Materials Management will walk each supply location
  3.  The tech will eyeball the amount of items in each bin and guesstimate about how much is needed to bring it back to max (or "par")
  4.  The tech goes back to the warehouse location, puts in an order or "requistion" for the items
  5.  The inventory is decremented if enough is on hand and a pick ticket is generated. The pick ticket will tell the staff member where 
      the items are located in the warehouse so they can easily find each item
  6.  Once all items are "picked", the tech goes to each floor and refills each bin

This project will focus on eliminating steps 2-4 resulting allowing more time for Materials Management staff concentrate on other duties.

#Solution

To eliminate the steps as mentioned above, I propose utilizing the <a href="http://falconfastening.com/lean-learning/inventory-management/basics-of-the-two-bin-kanban-system/">Kanban System</a>
and attaching removeable RFID cards to each item bin in the supply closets. Upon seeing an empty bin, the Nurse will simply swipe the 
RFID card of the item infront of the AutoREQ RFID reader and place it in the AutoREQ container. A signal will be sent from the the AutoREQ
system to Materials Management notifying them of the empty bin that needs to be refilled. 

#Technologies

This is still a work in progress but at the moment I'm using the following:

  <ul>
    <li>Arduino UNO</li>
    <li>Arduino WiFly</li>
    <li>RFID reader for Arduino</li>
    <li>Python</li>
    <li>MySql</li>
  </ul>
