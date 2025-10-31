# ⚡Adaptateur embarqué universel CAN/SPI/UART
Ce projet vise à développer un adaptateur embarqué universel capable de communiquer avec 
différents protocoles CAN, SPI et UART, basé sur un microcontrôleur STM32F407VGTx.
Le système peut recevoir des données sur un protocole et les transmettre sur un autre de
manière flexible, offrant ainsi une interface modulable pour des systèmes embarqués multi-
protocoles.

# 📌 Objectifs du projet
**.** Permettre la communication entre différents bus (CAN, SPI, UART).
**.** Fournir une architecture flexible pour rediriger les données d’un protocole vers un autre.
**.** Gérer les interruptions et événements en temps réel pour chaque interface.
**.** Démontrer l’utilisation des periphériques STM32 (CAN, SPI, UART, GPIO).
**.** Fournir une base évolutive pour des applications de passerelle embarquée ou d’interface multi-protocoles.

# 🛠 Fonctionnalités
Réception et transmission sur SPI (Maître/Esclave).
Réception et transmission sur CAN (filtrage configurable).
Réception et transmission sur UART (interruptions pour données entrantes).
Gestion des LEDs de statut pour indiquer la réception/transmission de données.
Routage des données entre les protocoles selon la variable data_output :
   - DEST_CAN
   - DEST_UART
   - DEST_SPI

# 🔧 Exemple de flux de données
**1- Données SPI reçues** → envoyées vers CAN ou UART selon configuration.

**2- Données CAN reçues** → envoyées vers SPI ou UART selon configuration.

**3- Données UART reçues** → envoyées vers SPI ou CAN selon configuration.

**.** Les données sont transférées en temps réel avec indication via LEDs.


# 🤝 Contribution
Les contributions sont les bienvenues !
N’hésitez pas à ouvrir une issue ou à proposer une pull request pour améliorer le projet.

# 📧 Contact
Pour toute question, suggestion ou collaboration, vous pouvez me contacter via :

Email : baker.essid98@gmail.com

LinkedIn : [Baker Essid](https://www.linkedin.com/in/baker-essid-b27b311b9/overlay/about-this-profile/?lipi=urn%3Ali%3Apage%3Ad_flagship3_profile_view_base%3Bgh8EYV5MTL%2BDU11rWtcMPA%3D%3D)

Je répondrai avec plaisir à vos messages et discussions autour du projet.
