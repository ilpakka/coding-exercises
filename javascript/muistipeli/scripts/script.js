class Muistipeli {
    constructor() {
        this.kortit = [];
        this.kaannetytKortit = [];
        this.loydettyParit = 0;
        this.siirrot = 0;
        this.pisteet = 0;
        this.lukittu = false;
        this.vaikeustaso = 'keskitaso';
        
        // DOM elementit
        this.pelilauta = document.getElementById('gameBoard');
        this.pisteetElementti = document.getElementById('score');
        this.siirrotElementti = document.getElementById('moves');
        this.uusiPeliNappi = document.getElementById('newGameBtn');
        this.vaikeustasoValinta = document.getElementById('difficulty');
        this.peliLoppuRuutu = document.getElementById('gameOver');
        this.lopullisetPisteetElementti = document.getElementById('finalScore');
        this.lopullisetSiirrotElementti = document.getElementById('finalMoves');
        this.pelaaUudelleenNappi = document.getElementById('playAgainBtn');

        // Tarkista DOM elementit
        if (!this.pelilauta || !this.pisteetElementti || !this.siirrotElementti || 
            !this.uusiPeliNappi || !this.vaikeustasoValinta || !this.peliLoppuRuutu || 
            !this.lopullisetPisteetElementti || !this.lopullisetSiirrotElementti || !this.pelaaUudelleenNappi) {
            throw new Error('Vaaditut DOM elementit puuttuvat');
        }

        // Tapahtumankäsittelijät
        this.sidottuAloitaUusiPeli = this.aloitaUusiPeli.bind(this);
        this.sidottuKäsitteleVaikeustasonMuutos = this.käsitteleVaikeustasonMuutos.bind(this);
        this.sidottuKäsittelePelaaUudelleen = this.käsittelePelaaUudelleen.bind(this);
        
        this.uusiPeliNappi.addEventListener('click', this.sidottuAloitaUusiPeli);
        this.vaikeustasoValinta.addEventListener('change', this.sidottuKäsitteleVaikeustasonMuutos);
        this.pelaaUudelleenNappi.addEventListener('click', this.sidottuKäsittelePelaaUudelleen);

        // Näppäimistönavigointi
        this.pelilauta.addEventListener('keydown', this.käsitteleNäppäimistönavigointi.bind(this));

        // Pelin aloitus
        this.aloitaUusiPeli();
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
            helppo: { parit: 6, sarakkeet: 4 },
            keskitaso: { parit: 8, sarakkeet: 4 },
            vaikea: { parit: 12, sarakkeet: 6 }
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
                img.src = `assets/card${arvo}.png`;
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
            kortinEtupuoli.innerHTML = `<img src="assets/card${arvo}.png" alt="Kortti ${arvo}" loading="lazy">`;
            
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
            this.käsittelePari();
        } else {
            this.käsitteleEriPari();
        }
    }

    käsittelePari() {
        this.pisteet += 10;
        this.pisteetElementti.textContent = this.pisteet;
        this.loydettyParit++;

        this.kaannetytKortit.forEach(kortti => {
            kortti.classList.add('loydetty');
            kortti.setAttribute('aria-label', 'Löydetty pari');
        });

        this.kaannetytKortit = [];
        this.lukittu = false;

        if (this.loydettyParit === this.haeVaikeustasonAsetukset().parit) {
            this.lopetaPeli();
        }
    }

    käsitteleEriPari() {
        setTimeout(() => {
            this.kaannetytKortit.forEach(kortti => {
                kortti.classList.remove('kaannetty');
            });
            this.kaannetytKortit = [];
            this.lukittu = false;
        }, 1000);
    }

    lopetaPeli() {
        this.lopullisetPisteetElementti.textContent = this.pisteet;
        this.lopullisetSiirrotElementti.textContent = this.siirrot;
        this.peliLoppuRuutu.classList.remove('piilotettu');
        this.peliLoppuRuutu.setAttribute('aria-live', 'polite');
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
        this.pisteet = 0;
        this.lukittu = false;

        this.pisteetElementti.textContent = this.pisteet;
        this.siirrotElementti.textContent = this.siirrot;
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