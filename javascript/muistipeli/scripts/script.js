class Muistipeli {
    constructor() {
        this.kortit = [];
        this.kaannetytKortit = [];
        this.loydettyParit = 0;
        this.siirrot = 0;
        this.lukittu = false;
        this.vaikeustaso = 'helppo';
        this.peliMoodi = 'single';
        this.nykyinenPelaaja = 1;
        this.pelaajaPisteet = [0, 0];
        this.pelaajaNimet = ['Pelaaja 1', 'Pelaaja 2'];
        
        // DOM elementit
        this.pelaajaAsetukset = document.getElementById('playerSetup');
        this.peliNaytto = document.getElementById('gameScreen');
        this.pelilauta = document.getElementById('gameBoard');
        this.pelaaja1Nimi = document.getElementById('player1Name');
        this.pelaaja2Nimi = document.getElementById('player2Name');
        this.pelaaja1Naytto = document.getElementById('player1NameDisplay');
        this.pelaaja2Naytto = document.getElementById('player2NameDisplay');
        this.pelaaja1Pisteet = document.getElementById('player1Score');
        this.pelaaja2Pisteet = document.getElementById('player2Score');
        this.nykyinenPelaajaNaytto = document.querySelector('#currentPlayer span');
        this.pelaaja1Info = document.getElementById('player1Info');
        this.pelaaja2Info = document.getElementById('player2Info');
        this.siirrotElementti = document.getElementById('moves');
        this.uusiPeliNappi = document.getElementById('newGameBtn');
        this.vaikeustasoValinta = document.getElementById('difficulty');
        this.peliLoppuRuutu = document.getElementById('gameOver');
        this.voittajaInfo = document.getElementById('winnerInfo');
        this.pelaaja1NimiFinal = document.getElementById('player1NameFinal');
        this.pelaaja2NimiFinal = document.getElementById('player2NameFinal');
        this.pelaaja1PisteetFinal = document.getElementById('player1ScoreFinal');
        this.pelaaja2PisteetFinal = document.getElementById('player2ScoreFinal');
        this.lopullisetSiirrotElementti = document.getElementById('finalMoves');
        this.pelaaUudelleenNappi = document.getElementById('playAgainBtn');
        this.aloitaPeliNappi = document.getElementById('startGameBtn');

        // Tarkista DOM elementit
        if (!this.pelilauta || !this.pelaaja1Nimi || !this.pelaaja2Nimi || 
            !this.uusiPeliNappi || !this.vaikeustasoValinta || !this.peliLoppuRuutu || 
            !this.aloitaPeliNappi) {
            throw new Error('Vaaditut DOM elementit puuttuvat');
        }

        // Tapahtumankäsittelijät
        this.sidottuAloitaUusiPeli = this.aloitaUusiPeli.bind(this);
        this.sidottuKäsitteleVaikeustasonMuutos = this.käsitteleVaikeustasonMuutos.bind(this);
        this.sidottuKäsittelePelaaUudelleen = this.käsittelePelaaUudelleen.bind(this);
        this.sidottuAloitaPeli = this.aloitaPeli.bind(this);
        
        this.aloitaPeliNappi.addEventListener('click', this.sidottuAloitaPeli);
        this.uusiPeliNappi.addEventListener('click', this.sidottuAloitaUusiPeli);
        this.vaikeustasoValinta.addEventListener('change', this.sidottuKäsitteleVaikeustasonMuutos);
        this.pelaaUudelleenNappi.addEventListener('click', this.sidottuKäsittelePelaaUudelleen);

        // Näppäimistönavigointi
        this.pelilauta.addEventListener('keydown', this.käsitteleNäppäimistönavigointi.bind(this));
    }

    aloitaPeli() {
        // Hae pelaajien nimet
        this.pelaajaNimet[0] = this.pelaaja1Nimi.value || 'Pelaaja 1';
        this.pelaajaNimet[1] = this.pelaaja2Nimi.value || 'Pelaaja 2';
        
        // Päivitä näytöllä olevat nimet
        this.pelaaja1Naytto.textContent = this.pelaajaNimet[0];
        this.pelaaja2Naytto.textContent = this.pelaajaNimet[1];
        this.pelaaja1NimiFinal.textContent = this.pelaajaNimet[0];
        this.pelaaja2NimiFinal.textContent = this.pelaajaNimet[1];

        // Tarkista pelimoodi
        this.peliMoodi = document.querySelector('input[name="gameMode"]:checked').value;
        
        // Näytä/piilota toisen pelaajan tiedot
        const player2Elements = document.querySelectorAll('#player2NameDisplay, #player2Score');
        const erotin = document.querySelector('.pelaaja-erotin');
        if (this.peliMoodi === 'multi') {
            player2Elements.forEach(el => el.classList.remove('piilotettu'));
            erotin.classList.remove('piilotettu');
        } else {
            player2Elements.forEach(el => el.classList.add('piilotettu'));
            erotin.classList.add('piilotettu');
        }

        // Siirry pelinäkymään
        this.pelaajaAsetukset.classList.add('piilotettu');
        this.peliNaytto.classList.remove('piilotettu');

        // Aloita peli
        this.aloitaUusiPeli();
    }

    päivitäNykyinenPelaaja() {
        this.nykyinenPelaaja = this.nykyinenPelaaja === 1 ? 2 : 1;
        this.nykyinenPelaajaNaytto.textContent = this.pelaajaNimet[this.nykyinenPelaaja - 1];
        
        // Päivitä aktiivinen pelaaja
        const player1Name = document.getElementById('player1NameDisplay');
        const player2Name = document.getElementById('player2NameDisplay');
        player1Name.classList.toggle('active', this.nykyinenPelaaja === 1);
        player2Name.classList.toggle('active', this.nykyinenPelaaja === 2);
    }

    käsitteleVaikeustasonMuutos(e) {
        this.vaikeustaso = e.target.value;
        this.aloitaUusiPeli();
    }

    käsittelePelaaUudelleen() {
        this.peliLoppuRuutu.classList.add('piilotettu');
        this.aloitaUusiPeli();
    }

    käsitteleNäppäimistönavigointi(e) {
        if (this.lukittu) return;
        
        const aktiivinenElementti = document.activeElement;
        if (!aktiivinenElementti.classList.contains('kortti')) return;

        const nykyinenIndeksi = this.kortit.indexOf(aktiivinenElementti);
        let seuraavaIndeksi;

        switch(e.key) {
            case 'ArrowRight':
                seuraavaIndeksi = nykyinenIndeksi + 1;
                break;
            case 'ArrowLeft':
                seuraavaIndeksi = nykyinenIndeksi - 1;
                break;
            case 'ArrowUp':
                seuraavaIndeksi = nykyinenIndeksi - this.haeVaikeustasonAsetukset().sarakkeet;
                break;
            case 'ArrowDown':
                seuraavaIndeksi = nykyinenIndeksi + this.haeVaikeustasonAsetukset().sarakkeet;
                break;
            case 'Enter':
            case ' ':
                e.preventDefault();
                this.kaannaKortti(aktiivinenElementti);
                return;
            default:
                return;
        }

        if (seuraavaIndeksi >= 0 && seuraavaIndeksi < this.kortit.length) {
            this.kortit[seuraavaIndeksi].focus();
        }
    }

    haeVaikeustasonAsetukset() {
        const asetukset = {
            helppo: { parit: 6, sarakkeet: 6 },
            keskitaso: { parit: 8, sarakkeet: 8 },
            vaikea: { parit: 12, sarakkeet: 8 }
        };
        return asetukset[this.vaikeustaso];
    }

    async luoKortit() {
        const asetukset = this.haeVaikeustasonAsetukset();
        const korttiArvot = [];
        
        // Korttiparien luonti
        for (let i = 1; i <= asetukset.parit; i++) {
            korttiArvot.push(i, i);
        }

        // Korttien sekoitus
        for (let i = korttiArvot.length - 1; i > 0; i--) {
            const j = Math.floor(Math.random() * (i + 1));
            [korttiArvot[i], korttiArvot[j]] = [korttiArvot[j], korttiArvot[i]];
        }

        // Korttien elementtien luonti
        this.pelilauta.style.gridTemplateColumns = `repeat(${asetukset.sarakkeet}, 1fr)`;
        this.pelilauta.innerHTML = '';

        // Kuvien esilataus
        const kuvaLupaukset = korttiArvot.map(arvo => 
            new Promise((resolve, reject) => {
                const img = new Image();
                img.onload = resolve;
                img.onerror = reject;
                img.src = `images/kortti${arvo}.jpeg`;
            })
        );

        try {
            await Promise.all(kuvaLupaukset);
        } catch (error) {
            console.error('Joidenkin korttien kuvien lataus epäonnistui:', error);
            // Jatka peliä, mutta osa kuvista saattaa olla latautumatta
        }

        korttiArvot.forEach((arvo, indeksi) => {
            const kortti = document.createElement('div');
            kortti.className = 'kortti';
            kortti.dataset.value = arvo;
            kortti.setAttribute('role', 'button');
            kortti.setAttribute('tabindex', '0');
            kortti.setAttribute('aria-label', `Kortti ${arvo}`);
            
            const kortinEtupuoli = document.createElement('div');
            kortinEtupuoli.className = 'kortin-etupuoli';
            kortinEtupuoli.innerHTML = `<img src="images/kortti${arvo}.jpeg" alt="Kortti ${arvo}" loading="lazy">`;
            
            const kortinTakapuoli = document.createElement('div');
            kortinTakapuoli.className = 'kortin-takapuoli';
            
            kortti.appendChild(kortinEtupuoli);
            kortti.appendChild(kortinTakapuoli);
            
            kortti.addEventListener('click', () => this.kaannaKortti(kortti));
            this.pelilauta.appendChild(kortti);
        });

        this.kortit = Array.from(this.pelilauta.children);
    }

    kaannaKortti(kortti) {
        if (this.lukittu || 
            this.kaannetytKortit.length >= 2 || 
            kortti.classList.contains('kaannetty') || 
            kortti.classList.contains('loydetty')) {
            return;
        }

        kortti.classList.add('kaannetty');
        this.kaannetytKortit.push(kortti);

        if (this.kaannetytKortit.length === 2) {
            this.siirrot++;
            this.siirrotElementti.textContent = this.siirrot;
            this.tarkistaPari();
        }
    }

    tarkistaPari() {
        this.lukittu = true;
        const [kortti1, kortti2] = this.kaannetytKortit;
        const pari = kortti1.dataset.value === kortti2.dataset.value;

        if (pari) {
            this.pelaajaPisteet[this.nykyinenPelaaja - 1] += 10;
            this.pelaaja1Pisteet.textContent = this.pelaajaPisteet[0];
            this.pelaaja2Pisteet.textContent = this.pelaajaPisteet[1];
            this.loydettyParit++;

            this.kaannetytKortit.forEach(kortti => {
                kortti.classList.add('loydetty');
                kortti.setAttribute('aria-label', 'Löydetty pari');
            });

            setTimeout(() => {
                this.kaannetytKortit = [];
                this.lukittu = false;

                if (this.loydettyParit === this.haeVaikeustasonAsetukset().parit) {
                    this.lopetaPeli();
                }
            }, 800);
        } else {
            setTimeout(() => {
                this.kaannetytKortit.forEach(kortti => {
                    kortti.classList.remove('kaannetty');
                });
                this.kaannetytKortit = [];
                this.lukittu = false;
                
                if (this.peliMoodi === 'multi') {
                    this.päivitäNykyinenPelaaja();
                }
            }, 1000);
        }
    }

    lopetaPeli() {
        this.pelaaja1PisteetFinal.textContent = this.pelaajaPisteet[0];
        this.pelaaja2PisteetFinal.textContent = this.pelaajaPisteet[1];
        this.lopullisetSiirrotElementti.textContent = this.siirrot;

        // Määritä voittaja
        if (this.peliMoodi === 'multi') {
            if (this.pelaajaPisteet[0] > this.pelaajaPisteet[1]) {
                this.voittajaInfo.textContent = `${this.pelaajaNimet[0]} voitti!`;
            } else if (this.pelaajaPisteet[1] > this.pelaajaPisteet[0]) {
                this.voittajaInfo.textContent = `${this.pelaajaNimet[1]} voitti!`;
            } else {
                this.voittajaInfo.textContent = 'Tasapeli!';
            }
        } else {
            this.voittajaInfo.textContent = 'Onnittelut! Kaikki parit löydetty!';
        }

        this.peliLoppuRuutu.classList.remove('piilotettu');
        this.peliLoppuRuutu.setAttribute('aria-live', 'polite');

        // Confetti effect
        const duration = 3 * 1000;
        const animationEnd = Date.now() + duration;
        const defaults = { startVelocity: 30, spread: 360, ticks: 60, zIndex: 0 };

        function randomInRange(min, max) {
            return Math.random() * (max - min) + min;
        }

        const interval = setInterval(function() {
            const timeLeft = animationEnd - Date.now();

            if (timeLeft <= 0) {
                return clearInterval(interval);
            }

            const particleCount = 50 * (timeLeft / duration);
            
            confetti({
                ...defaults,
                particleCount,
                origin: { x: randomInRange(0.1, 0.3), y: Math.random() - 0.2 }
            });
            
            confetti({
                ...defaults,
                particleCount,
                origin: { x: randomInRange(0.7, 0.9), y: Math.random() - 0.2 }
            });
        }, 250);
    }

    async aloitaUusiPeli() {
        // Poista vanhat tapahtumankäsittelijät
        this.kortit.forEach(kortti => {
            kortti.removeEventListener('click', () => this.kaannaKortti(kortti));
        });

        this.kortit = [];
        this.kaannetytKortit = [];
        this.loydettyParit = 0;
        this.siirrot = 0;
        this.lukittu = false;
        this.nykyinenPelaaja = 1;
        this.pelaajaPisteet = [0, 0];

        this.pelaaja1Pisteet.textContent = '0';
        this.pelaaja2Pisteet.textContent = '0';
        this.siirrotElementti.textContent = '0';
        this.nykyinenPelaajaNaytto.textContent = this.pelaajaNimet[0];
        
        // Päivitä aktiivinen pelaaja
        const player1Name = document.getElementById('player1NameDisplay');
        const player2Name = document.getElementById('player2NameDisplay');
        player1Name.classList.add('active');
        player2Name.classList.remove('active');
        
        this.peliLoppuRuutu.classList.add('piilotettu');

        await this.luoKortit();
    }

    // Siivousmetodi
    tuhoa() {
        this.uusiPeliNappi.removeEventListener('click', this.sidottuAloitaUusiPeli);
        this.vaikeustasoValinta.removeEventListener('change', this.sidottuKäsitteleVaikeustasonMuutos);
        this.pelaaUudelleenNappi.removeEventListener('click', this.sidottuKäsittelePelaaUudelleen);
        this.pelilauta.removeEventListener('keydown', this.käsitteleNäppäimistönavigointi);
    }
}

// Alusta peli kun DOM on ladattu
document.addEventListener('DOMContentLoaded', () => {
    const peli = new Muistipeli();
    
    // Siivous kun sivu suljetaan
    window.addEventListener('unload', () => {
        peli.tuhoa();
    });
}); 