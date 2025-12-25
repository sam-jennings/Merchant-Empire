// ARCHMAGE ASCENSION - GAME CONFIGURATION
// Modify these values to change game rules without touching the app code

const GameConfig = {
  // ============================================================================
  // GAME METADATA
  // ============================================================================
  gameName: "Archmage Ascension",
  gameSubtitle: "The Ascension Trial",
  pointsName: "Recognition Points", // Short: "RP"
  pointsAbbreviation: "RP",

  // ============================================================================
  // ELEMENTS/DOMAINS
  // ============================================================================
  elements: [
    { id: 'earth', name: 'Earth', symbol: '♠' },
    { id: 'fire', name: 'Fire', symbol: '♥' },
    { id: 'water', name: 'Water', symbol: '♦' },
    { id: 'air', name: 'Air', symbol: '♣' }
  ],

  domains: {
    // Regular domains
    earth: { name: 'Elemental Power (♠ Earth)', category: 'elemental' },
    fire: { name: 'Elemental Power (♥ Fire)', category: 'elemental' },
    water: { name: 'Elemental Power (♦ Water)', category: 'elemental' },
    air: { name: 'Elemental Power (♣ Air)', category: 'elemental' },
    transformation: { name: 'Transformation Power', category: 'special' },
    enchantment: { name: 'Enchantment Power', category: 'special' },
    
    // Wild domain
    wilds: { name: 'Wild Magic Power', category: 'wild' }
  },

  // ============================================================================
  // SPELL TYPES
  // ============================================================================
  spellTypes: [
    {
      id: 'conjuration',
      name: 'Conjuration',
      emoji: '🔮',
      requiresElement: true,
      minSize: 3,
      maxSize: 12,
      warnUnder: 3,
      warnOver: 9,
      // Function that adds power when this spell is created
      contributeToDomains: (spell, domains, calculatePower) => {
        const elementKey = spell.element.toLowerCase();
        const power = calculatePower(spell.size);
        domains[elementKey] += power;
      }
    },
    {
      id: 'transfiguration',
      name: 'Transfiguration',
      emoji: '🔄',
      requiresElement: false,
      minSize: 3,
      maxSize: 12,
      warnUnder: 3,
      warnOver: 9,
      contributeToDomains: (spell, domains, calculatePower) => {
        const power = calculatePower(spell.size);
        domains.transformation += power;
      }
    },
    {
      id: 'enchantment',
      name: 'Enchantment',
      emoji: '⚡',
      requiresElement: false,
      minSize: 3,
      maxSize: 4,
      warnUnder: 3,
      warnOver: 4,
      contributeToDomains: (spell, domains, calculatePower) => {
        const power = calculatePower(spell.size);
        domains.enchantment += power;
        if (spell.size === 4) {
          domains.wilds += 1;
        }
      }
    },
    {
      id: 'perfect-transmutation',
      name: 'Perfect Transmutation',
      emoji: '✨',
      requiresElement: true,
      minSize: 3,
      maxSize: 12,
      warnUnder: 3,
      warnOver: 9,
      contributeToDomains: (spell, domains, calculatePower) => {
        const elementKey = spell.element.toLowerCase();
        const power = calculatePower(spell.size);
        domains[elementKey] += power;
        domains.transformation += power;
        domains.wilds += 1;
      }
    }
  ],

  // ============================================================================
  // POWER CALCULATION FORMULA
  // ============================================================================
  // This function determines how spell size converts to power
  calculatePower: (size) => {
    if (size <= 4) return size * 1;
    if (size <= 6) return Math.round(size * 1.5);
    return size * 2;
  },

  // Helper to get the formula description for display
  getPowerBreakdown: (size, calculatePower) => {
    const power = calculatePower(size);
    if (size <= 4) return `${size} × 1 = ${power}`;
    if (size <= 6) return `${size} × 1.5 = ${power}`;
    return `${size} × 2 = ${power}`;
  },

  powerFormulaDescription: "3-4 components ×1, 5-6 components ×1.5, 7+ components ×2",

  // ============================================================================
  // TRIALS
  // ============================================================================
  trials: [
    {
      id: 'elemental-purity',
      name: 'Elemental Purity Trial',
      description: 'Power from EXACTLY ONE elemental domain',
      icon: 'Zap',
      // Function that returns available domains based on current allocation
      getAvailableDomains: (currentSources) => {
        const elementalDomains = ['earth', 'fire', 'water', 'air'];
        const currentRegular = currentSources.filter(d => d !== 'wilds');
        
        if (currentRegular.length === 0) {
          return elementalDomains;
        } else {
          return currentRegular.filter(d => elementalDomains.includes(d));
        }
      },
      // Function that validates if an allocation is legal
      isValidAllocation: (domainsUsed, elementalDomainsUsed) => {
        return domainsUsed.length === 1 && elementalDomainsUsed.length === 1;
      },
      // Maximum sources allowed (for validation during input)
      maxSources: 1
    },
    {
      id: 'elemental-harmony',
      name: 'Elemental Harmony Trial',
      description: 'Power from 2+ DIFFERENT elemental domains',
      icon: 'Users',
      getAvailableDomains: (currentSources) => {
        return ['earth', 'fire', 'water', 'air'];
      },
      isValidAllocation: (domainsUsed, elementalDomainsUsed) => {
        return elementalDomainsUsed.length >= 2;
      },
      maxSources: 4,
      usesBreadthTiebreaker: true
    },
    {
      id: 'transformation',
      name: 'Transformation Trial',
      description: 'Power from Transformation domain only',
      icon: 'Sparkles',
      getAvailableDomains: (currentSources) => {
        return ['transformation'];
      },
      isValidAllocation: (domainsUsed, elementalDomainsUsed) => {
        return domainsUsed.length === 1 && domainsUsed[0] === 'transformation';
      },
      maxSources: 1
    },
    {
      id: 'enchantment',
      name: 'Enchantment Trial',
      description: 'Power from Enchantment domain only',
      icon: 'Trophy',
      getAvailableDomains: (currentSources) => {
        return ['enchantment'];
      },
      isValidAllocation: (domainsUsed, elementalDomainsUsed) => {
        return domainsUsed.length === 1 && domainsUsed[0] === 'enchantment';
      },
      maxSources: 1
    },
    {
      id: 'focused-power',
      name: 'Focused Power Trial',
      description: 'Power from EXACTLY ONE domain (any type)',
      icon: 'Crown',
      getAvailableDomains: (currentSources) => {
        const allDomains = ['earth', 'fire', 'water', 'air', 'transformation', 'enchantment'];
        const currentRegular = currentSources.filter(d => d !== 'wilds');
        
        if (currentRegular.length === 0) {
          return allDomains;
        } else {
          return currentRegular;
        }
      },
      isValidAllocation: (domainsUsed, elementalDomainsUsed) => {
        return domainsUsed.length === 1;
      },
      maxSources: 1
    },
    {
      id: 'universal-power',
      name: 'Universal Power Trial',
      description: 'Power from 2+ DIFFERENT domains (any mix)',
      icon: 'Users',
      getAvailableDomains: (currentSources) => {
        return ['earth', 'fire', 'water', 'air', 'transformation', 'enchantment'];
      },
      isValidAllocation: (domainsUsed, elementalDomainsUsed) => {
        return domainsUsed.length >= 2;
      },
      maxSources: 6,
      usesBreadthTiebreaker: true
    },
    {
      id: 'complete-mastery',
      name: 'Complete Mastery Trial',
      description: 'Power from ALL THREE domain types (at least one element + Transformation + Enchantment)',
      icon: 'Star',
      minPlayers: 5,
      getAvailableDomains: (currentSources) => {
        return ['earth', 'fire', 'water', 'air', 'transformation', 'enchantment'];
      },
      isValidAllocation: (domainsUsed, elementalDomainsUsed) => {
        const hasElement = elementalDomainsUsed.length >= 1;
        const hasTransformation = domainsUsed.includes('transformation');
        const hasEnchantment = domainsUsed.includes('enchantment');
        return hasElement && hasTransformation && hasEnchantment;
      },
      maxSources: 6,
      usesBreadthTiebreaker: true
    }
  ],

  // ============================================================================
  // RECOGNITION POINTS (VICTORY POINTS) TABLE
  // ============================================================================
  recognitionPoints: {
    2: {
      'elemental-purity': { 1: 6 },
      'elemental-harmony': { 1: 4 },
      'transformation': { 1: 7 },
      'enchantment': { 1: 4 },
      'focused-power': { 1: 6 },
      'universal-power': { 1: 3 }
    },
    3: {
      'elemental-purity': { 1: 8 },
      'elemental-harmony': { 1: 5 },
      'transformation': { 1: 9 },
      'enchantment': { 1: 5 },
      'focused-power': { 1: 7 },
      'universal-power': { 1: 4 }
    },
    4: {
      'elemental-purity': { 1: 8 },
      'elemental-harmony': { 1: 6 },
      'transformation': { 1: 9 },
      'enchantment': { 1: 6 },
      'focused-power': { 1: 7 },
      'universal-power': { 1: 5 }
    },
    5: {
      'elemental-purity': { 1: 8 },
      'elemental-harmony': { 1: 6 },
      'transformation': { 1: 9 },
      'enchantment': { 1: 6 },
      'focused-power': { 1: 7 },
      'universal-power': { 1: 5 },
      'complete-mastery': { 1: 6 }
    },
    6: {
      'elemental-purity': { 1: 7, 2: 3 },
      'elemental-harmony': { 1: 6, 2: 2 },
      'transformation': { 1: 8, 2: 3 },
      'enchantment': { 1: 6, 2: 2 },
      'focused-power': { 1: 7, 2: 3 },
      'universal-power': { 1: 5, 2: 2 },
      'complete-mastery': { 1: 6, 2: 2 }
    },
    7: {
      'elemental-purity': { 1: 7, 2: 4 },
      'elemental-harmony': { 1: 6, 2: 3 },
      'transformation': { 1: 8, 2: 4 },
      'enchantment': { 1: 6, 2: 3 },
      'focused-power': { 1: 7, 2: 4 },
      'universal-power': { 1: 5, 2: 3 },
      'complete-mastery': { 1: 6, 2: 3 }
    }
  },

  // Recognition Points by Spell Size (for Simple Scoring mode)
  recognitionPointsBySpellSize: {
    conjuration: { 3: 4, 4: 6, 5: 9, 6: 13, 7: 18, 8: 24, 9: 31, 10: 39, 11: 48, 12: 58 },
    transfiguration: { 3: 5, 4: 8, 5: 12, 6: 17, 7: 23, 8: 30, 9: 38, 10: 47, 11: 57, 12: 68 },
    enchantment: { 3: 6, 4: 14 },
    'perfect-transmutation': { 3: 7, 4: 11, 5: 16, 6: 23, 7: 32, 8: 42, 9: 54, 10: 68, 11: 84, 12: 102 }
  },

  // ============================================================================
  // THEME/COLORS
  // ============================================================================
  colors: {
    light: {
      primary: 'purple',
      secondary: 'indigo',
      accent: 'pink',
      success: 'green',
      warning: 'yellow',
      danger: 'red',
      
      // Gradient backgrounds
      background: 'from-indigo-50 via-purple-50 to-pink-50',
      cardAccent: 'from-purple-100 to-pink-100',
      
      // Specific colors
      headerText: 'purple-900',
      subheaderText: 'purple-700',
      bodyText: 'gray-900',
      mutedText: 'gray-600',
      border: 'gray-300',
      cardBg: 'white'
    },
    dark: {
      primary: 'purple',
      secondary: 'indigo',
      accent: 'pink',
      success: 'green',
      warning: 'yellow',
      danger: 'red',
      
      // Gradient backgrounds
      background: 'from-gray-900 via-purple-900 to-indigo-900',
      cardAccent: 'from-purple-800 to-indigo-800',
      
      // Specific colors
      headerText: 'purple-300',
      subheaderText: 'purple-400',
      bodyText: 'gray-100',
      mutedText: 'gray-400',
      border: 'gray-600',
      cardBg: 'gray-800'
    }
  },

  // ============================================================================
  // UI TEXT
  // ============================================================================
  text: {
    playerNoun: 'Wizard',
    playerNounPlural: 'Wizards',
    addPlayerButton: 'Add Wizard',
    playerInputPlaceholder: 'Wizard name',
    startGameButton: 'Start Ascension Trial',
    startGameRequirement: 'All wizards must enter at least one power point',
    
    calculatorButton: 'Spell Calculator',
    manualEntryButton: 'Manual Entry',
    manualEntryActive: '✓ Using Manual Entry',
    switchToManual: 'Switch to Manual Entry',
    
    setupHeader: 'Game Setup',
    setupSubheader: 'Calculate or Enter Domain Power',
    addPlayersHeader: 'Add Wizards',
    
    votingHeader: 'Power Allocation',
    remainingPowerHeader: 'Remaining Power',
    selectTrialHeader: 'Select Trial to Allocate Power',
    referenceTableHeader: 'Wizard Domain Power Reference',
    
    resultsHeader: 'Ascension Trial Results',
    finalStandingsHeader: 'Final Standings',
    
    lockButton: 'Lock In Allocations',
    unlockButton: 'Unlock Allocations',
    lockedMessage: 'Your allocations are locked. Unlock to make changes.',
    unallocatedWarning: 'You have unallocated power remaining',
    
    viewResultsButton: 'View Results',
    clearButton: 'Clear',
    removeButton: 'Remove',
    
    noValidAttempts: 'No valid attempts for this trial',
    winner: 'Winner',
    totalPower: 'Total Power',
    availablePower: 'Available',
    
    wildMagicNote: 'Wild magic can only be added to trials with existing regular power',
    
    darkMode: 'Dark Mode',
    lightMode: 'Light Mode'
  }
};

// Export for use in the main app
if (typeof module !== 'undefined' && module.exports) {
  module.exports = GameConfig;
}
